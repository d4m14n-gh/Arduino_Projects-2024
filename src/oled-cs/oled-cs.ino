#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
#define XP A1
#define YP A0

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  pinMode(8, INPUT);
  pinMode(8, OUTPUT);
  Serial.begin(9600);

  delay(250); 
  display.begin(i2c_Address, true); 
  display.display();
  display.setCursor(0, 0);
}

void drawString(String text, int posx=0, int posy=0);
void drawAmmo(int status=0);

void loop() {
  if(Serial.available()>0){
    String x = Serial.readStringUntil('\n');
    if (x=="start"){
      display.clearDisplay();
      display.setCursor(0, 0);
      while(true){
        String text = Serial.readStringUntil('\n');
        if(text=="end")
          break;
        else
          drawString(text);
      }
      drawAmmo(Serial.parseInt());
      display.display();
    }
  }
}

void drawString(String text, int posx=0, int posy=0){
  display.setTextSize(1);
  display.setTextColor(1);
  for (int i = 0; i < text.length(); i++)
    display.write(text[i]);
  display.write('\n');
}

void drawAmmo(int status=0){
  for (int i = 0; i < status; i++){
    display.fillRoundRect(i*12+4, SCREEN_HEIGHT-20, 7, 15, 5, 1);
    display.fillRect(i*12+4, SCREEN_HEIGHT-10, 7, 10, 1);
  }
}
