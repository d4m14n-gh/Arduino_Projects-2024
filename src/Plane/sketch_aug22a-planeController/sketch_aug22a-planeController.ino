#include <LiquidCrystal_I2C.h>
#include <MyArduino.h>
#include <LoRa.h>
#include "pins.h"
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo setter;

void writeLcd(String line1, String line2 = "^_~"){
  lcd.clear();
  lcd.home();
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void setup() {
  lcd.init();
  lcd.backlight();

  if (!LoRa.begin(433E6)) {
    digitalWrite(LED_BUILTIN, HIGH);
    while (1);
  }

  delay(1000);
  writeLcd("Starting...");
  setter.attach(3);
  setter.write(90);
}

void loop() {
  int jx = map(analogRead(A1), 0, 1023, 0, 181);
  int jy = map(analogRead(A2), 0, 1023, 0, 181);
  int speed = map(analogRead(A0), 0, 1023, 0, 181);
  int roll = map(analogRead(A3), 0, 1023, 0, 91);
  int pitch = jy/2;

  int angle1= 180-(jy-(jx-90));
  int angle2= 180-(jy+(jx-90));
  angle1=max(0, min(angle1, 180)); 
  angle2=max(0, min(angle2, 180));

  //if(newAngle1!=angle1||newAngle2!=angle2||newSpeed!=speed){
  writeLcd("v:"+String(speed)+" r:"+String(roll)+" p:"+String(pitch), "a1: "+String(angle1)+" a2: "+String(angle2));

  LoRa.beginPacket();
  LoRa.write((uint8_t)angle1);
  LoRa.write((uint8_t)angle2);
  LoRa.write((uint8_t)speed);
  LoRa.write((uint8_t)roll);
  LoRa.write((uint8_t)pitch);
  LoRa.write((uint8_t) ((angle1+angle2*2+speed*3+roll*5+pitch*7)%251) );
  LoRa.endPacket();
  //}
  
  setter.write(speed);
  delay(50);
}
