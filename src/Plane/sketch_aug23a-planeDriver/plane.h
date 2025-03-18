#include "WString.h"
#include <Arduino.h>
#include <math.h>
#include <HardwareSerial.h>

class Plane{
private:
  unsigned long timeout=0;
  unsigned long lastTargetUpdate=0;
  int ang1=0, ang2=0, ang3=0, ang4=0;
  int targetRoll=0, targetPitch=0;
  double roll=0, pitch=0, yaw=0;
  int speed=0;
public:
  //[timeout]=milliseconds
  Plane(unsigned long timeout){
    lastTargetUpdate=millis();
    this->timeout=timeout;
  }
  
  void start(){
    lastTargetUpdate=millis();  
  }

  void updateTarget(int targetRoll, int targetPitch, int targetSpeed){
    this->targetRoll=targetRoll;
    this->targetPitch=targetPitch;
    speed=targetSpeed;
    lastTargetUpdate=millis();
  }

  double sgn(double x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
  }

  int wingAngleFun(double x, double crossPoint=45, double exponent=1.3){
    if(abs(x)<crossPoint)
      return round( sgn(x)* (pow(abs(x), exponent)/pow(crossPoint, exponent-1)) ); 
    else
      return x;
  }

  void update(double planeRoll, double planePitch, double planeYaw){
    if(millis()-lastTargetUpdate>timeout){
      targetRoll=0;
      targetPitch=-5;
      speed=0;
    }

    roll=planeRoll;
    pitch=planePitch;
    yaw=planeYaw;
    ang1=90-  wingAngleFun(targetRoll-planeRoll)   -(targetPitch-planePitch);
    ang2=90+  wingAngleFun(targetRoll-planeRoll)   -(targetPitch-planePitch);
    ang1=max(0, min(180, ang1));
    ang2=max(0, min(180, ang2));
    ang3=90+(targetPitch-planePitch);
    ang4=90+(targetPitch-planePitch);
  }

  void printAngles(bool draw=false){
    static char buffer[20];
    Serial.print("tRoll:"+String(targetRoll)+" tPitch:"+String(targetPitch));
    Serial.print(" roll:"+String(roll)+" pitch:"+String(pitch)+" yaw:"+String(yaw));
    Serial.print(" a1:"+String(ang1)+" a2:"+String(ang2)+" a3:"+String(ang3)+" a4:"+String(ang4));
    Serial.println(" speed:"+String(speed));
    if(draw){
      Serial.println();
      Serial.println("      ^       ");
      Serial.println("     /|\\ ");
      Serial.println("     | |  ");
      Serial.println(".___/   \\___  ");
      sprintf(buffer, "/%02d__   __%02d\\.\0", ang1/2, ang2/2);
      Serial.println(buffer);
      Serial.println("     | |      ");
      Serial.println("    / ^ \\     ");
      sprintf(buffer, "  <%02d|_|%02d>   \0", ang3/2, ang4/2);
      Serial.println(buffer);
    }
  }

  int getAng1(){
    return ang1;
  }

  int getAng2(){
    return ang2;
  }

  int getAng3(){
    return ang3;
  }

  int getAng4(){
    return ang4;
  }

  int getSpeed(){
    return speed;
  }
};