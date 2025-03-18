/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <WiFi.h>
#include <NetworkClient.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT22.h>
#include <Wire.h>
#include <Adafruit_SH110X.h>
#include <time.h>

#define dhtPin 19
const int led = 23;

DHT22 dht(dhtPin);
float t = 0;
float h = 0;
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire, -1);

float lastT[150];

const char *ssid = "Livebox-4C38";
const char *password = "41266ba5884f7085f53630adfd4d3943bb7d45994d4b31634637fcc78dd0f415";
const char* ntpServer = "pool.ntp.org";
const long utcOffsetInSeconds = 2*3600;

WebServer server(80);


void handleRoot() {
  digitalWrite(led, 1);
  char temp[724];
  int sec = millis() / 1000;
  int hr = sec / 3600;
  int min = (sec / 60) % 60;
  sec = sec % 60;
  //<meta http-equiv='refresh' content='5'/>
  snprintf(
    temp, 724,

    "<html>\
      <head>\
        <meta http-equiv='refresh' content='38'/>\
        <title>ESP32 Demo</title>\
        <style>\
          body { background-color: #212121; font-family: Arial, Helvetica, Sans-Serif; Color: #ECECE6; }\
          h1 {background-color: #171717}\
          a {Color: crimson}\
        </style>\
      </head>\
      <body>\
        <h1><img src=\"https://cdn-icons-png.freepik.com/512/7339/7339985.png\" width=\"20\"/> Hello from ESP32!</h1>\
        <p>Uptime: %02d:%02d:%02d</p>\
        <p>temp: %.2f</p>\
        <p>hum: %.2f</p>\
        <a href=\"/test.svg\">wykres </a>\
        <br/>\
        <br/>\
        <a href=\"/\">refresh</a>\
        <br/>\
        <br/>\
        <img src=\"/test.svg\" />\
      </body>\
    </html>",

    hr, min, sec, t, h
  );
  server.send(200, "text/html", temp);
  delay(1000);
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  //Wire.setPins(4, 2);
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  display.begin(0x3c, true);
  display.setTextColor(1);
  display.display();

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/test.svg", drawGraph);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  configTime(utcOffsetInSeconds, 0, ntpServer);
}

const char* serverName = "http://192.168.1.19:5000/api/TemperatureReadInfoes";
char formattedTime[50]="\"2024-08-06T21:37:00.946\"\0";
void setTime(void){
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  snprintf(formattedTime, 50, "\"%04d-%02d-%02dT%02d:%02d:%02d.%03dZ\"",
           timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
           timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, 0);
  Serial.println(formattedTime);
}
void loop(void) {
  char temp[200];
  h = dht.getHumidity();
  t = dht.getTemperature();
  setTime();
  snprintf(
    temp, 200,
    "{\
      \"id\": 0,\
      \"readDate\": %s,\
      \"temperature\": %.2f,\
      \"humidity\": %.2f}\
    ",formattedTime, t, h
  );
  Serial.println(temp);
  for(int i=0;i<127;i++)
    lastT[i]=lastT[i+1];
  lastT[127]=t;
  display.clearDisplay();
  for(int i=0;i<128;i++){
    int y = (int)((lastT[i]-20)/10.0f*64.0f);
    display.drawPixel(i, display.height()-y, 1);
  }
  display.display();
  ////////////////////
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(temp);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }

  ////////////////////
  server.handleClient();
  delay(1*60*1000);  //allow the cpu to switch to other tasks
}

void drawGraph() {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(23, 23, 23)\" stroke-width=\"1\" stroke=\"#ECECE6\" />\n";
  out += "<g stroke=\"crimson\">\n";
  int y = (int)((lastT[0]-15)/20.0f*150.0f); //rand() % 130;
  for (int x = 1; x < 128; x++) {
    int y2 = (int)((lastT[x]-15)/20.0f*150.0f);
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x*3, 150 - y, x*3 + 3, 150 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}
