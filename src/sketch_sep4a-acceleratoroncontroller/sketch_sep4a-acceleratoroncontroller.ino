#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <LoRa.h>
#include "pins.h"
#define DEBUGGYRO
#include "acc.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo stest;


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

  stest.attach(4);
  stest.write(90);

  accelgyro::setAcc();

  delay(1000);
  writeLcd("Starting...");
}

int speed=0;
int angle1=0;
int angle2=0;

void loop() {
  accelgyro::readAcc();
  //Serial.print("Roll:"); Serial.print(accelgyro::roll); Serial.print(" ");
  //Serial.print("Pitch:"); Serial.print(accelgyro::pitch); Serial.print(" ");
  //Serial.print("Yaw:"); Serial.println(accelgyro::yaw);

  int newSpeed = map(analogRead(A0), 0, 1023, 0, 180);
  int jx = map(analogRead(A1), 0, 1023, 0, 180);
  int jy = map(analogRead(A2), 0, 1023, 0, 180);

  int newAngle1= 180-(jy-(jx-90));
  int newAngle2= 180-(jy+(jx-90));
  newAngle1=max(0, min(newAngle1, 180)); 
  newAngle2=max(0, min(newAngle2, 180));

  //if(newAngle1!=angle1||newAngle2!=angle2||newSpeed!=speed){
    angle1=newAngle1;
    angle2=newAngle2;
    speed=newSpeed;
    writeLcd("speed: "+String(speed), "a1: "+String(angle1)+" a2: "+String(angle2));

    LoRa.beginPacket();
    LoRa.write((uint8_t)angle1);
    LoRa.write((uint8_t)angle2);
    LoRa.write((uint8_t)speed);
    LoRa.write((uint8_t) ((speed+angle1*2+angle2*3)%251));
    LoRa.endPacket();
  //}
  
  stest.write(angle1);

  delay(100);
}

