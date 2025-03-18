#include <Servo.h>
#include <SPI.h>
#include <LoRa.h>
#include <HardwareSerial.h>
#include "pins.h"
#define DEBUGGYRO 
#include "acc.h"
#include "plane.h"
#define INTERVAL 100  //<1000

Servo servo2;
Servo servo1;
Servo servo3;
Servo engine;

Plane plane(2500);

void setup() {
  Wire.begin();
  Serial.begin(9600);
  if (!LoRa.begin(433E6)) {
    digitalWrite(LED_BUILTIN, HIGH);
    while (1);
  }
  LoRa.setPins(10, 9, 8);

  servo1.attach(pin1);
  servo1.write(90);
  servo2.attach(pin2);
  servo2.write(90);
  servo3.attach(pin3);
  servo3.write(90);
  engine.attach(pinEngine, 1000, 2000);
  engine.writeMicroseconds(1000);
  accelgyro::setAcc();

  delay(1000);
  Serial.println("starting...");

  accelgyro::start(1000.0/INTERVAL);
  plane.start(); 
}

int newAngle1, newAngle2, newSpeed, newRoll, newPitch;
bool takePacket(bool debug=false){
  int sum251;
  int packetSize = LoRa.parsePacket();
  if (packetSize==6) {
    if(debug)
      Serial.print("Received packet with RSSI "+String(LoRa.packetRssi()));
    newAngle1=LoRa.read();
    newAngle2=LoRa.read();
    newSpeed=LoRa.read();
    newRoll=LoRa.read();
    newPitch=LoRa.read();
    sum251=LoRa.read();
    if(sum251==(newAngle1+newAngle2*2+newSpeed*3+newRoll*5+newPitch*7)%251){
      if(debug)
        Serial.println("packet succesfully read!");
      return true;
    }
    else{
      if(debug)
        Serial.println("checksum error!");
    }
  }
  return false;
}

void loop() {
  unsigned long lastM = millis(); 
  accelgyro::readAcc();
  plane.update(-accelgyro::pitch, -accelgyro::roll, accelgyro::yaw);
  if(takePacket())
    plane.updateTarget(newRoll-45, (newPitch-45)/2+5, newSpeed);

  plane.printAngles();  

  servo1.write(180-plane.getAng1());
  servo2.write(plane.getAng2());
  servo3.write(plane.getAng3());

  engine.writeMicroseconds(1000+(int)((float)(plane.getSpeed())/180.0f*1000.0f));

  unsigned long differ = millis()-lastM;
  if(differ < INTERVAL)  
    delay(INTERVAL-differ);
  else
    Serial.println(differ);
}
