#include <WiFi.h>
#include <NetworkClient.h>
#include <WebServer.h>

const char *ssid = "Livebox-4C38";
const char *password = "41266ba5884f7085f53630adfd4d3943bb7d45994d4b31634637fcc78dd0f415";

WebServer server(80);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    server.send(200, "text/plain", "hello world");
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  delay(100);
}
