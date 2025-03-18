#include <MPU6050.h>
#include <MadgwickAHRS.h>

namespace accelgyro{

  static MPU6050 accelgyro(0x68);
  static Madgwick ahrs;
  static int16_t ax, ay, az;
  static float axo, ayo, azo;
  static float gxo, gyo, gzo;
  static int16_t gx, gy, gz;
  float roll, pitch, yaw;



  void readAcc(){
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    float ax_g = (ax+axo) / 16384.0; // Przy założeniu ±2g zakres
    float ay_g = (ay+ayo) / 16384.0;
    float az_g = (az+azo) / 16384.0;
    float gx_dps = (gx+gxo) / 131.0; // Przy założeniu ±250°/s zakres
    float gy_dps = (gy+gyo) / 131.0;
    float gz_dps = (gz+gzo) / 131.0;
    ahrs.updateIMU(gx_dps, gy_dps, gz_dps, ax_g, ay_g, az_g);
    roll = ahrs.getRoll();
    pitch = ahrs.getPitch();
    yaw = ahrs.getYaw();
  }

  void calibrateGyro(int cnt, int intervalMili){
    Serial.println("starting gyro calibration dont move mpu!");
    delay(1000);
    accelgyro.setXGyroOffset((int16_t)0);
    accelgyro.setYGyroOffset((int16_t)0);
    accelgyro.setZGyroOffset((int16_t)0);
    delay(1000);
    int32_t gxm=0, gym=0, gzm=0;
    for(int i=0;i<cnt;i++){
      readAcc();
      gxm+=(int32_t)gx;
      gym+=(int32_t)gy;
      gzm+=(int32_t)gz;
      delay(intervalMili);
    }

    gxm/=cnt;
    gym/=cnt;
    gzm/=cnt;

    Serial.println(gxm);
    Serial.println(gym);
    Serial.println(gzm);

    accelgyro.setXGyroOffset(-(int16_t)gxm/4);
    accelgyro.setYGyroOffset(-(int16_t)gym/4);
    accelgyro.setZGyroOffset(-(int16_t)gzm/4);
    Serial.println("Gyroscope calibration done!");
  }

  void calibrateAcc(int cnt, int interval){
    accelgyro.setXAccelOffset(6000);
    accelgyro.setYAccelOffset(6000);
    accelgyro.setZAccelOffset(6000);
  
    Serial.println("starting accel calibration dont move mpu!");

    int32_t axm=0, aym=0, azm=0;
    for(int i=0;i<cnt;i++){
      readAcc();
      axm+=(int32_t)ax;
      aym+=(int32_t)ay;
      azm+=(int32_t)az;
      delay(interval);
    }

    axm/=cnt;
    aym/=cnt;
    azm/=cnt;

    //Serial.println(axm);
    //Serial.println(aym);
    //Serial.println(azm);

    
    Serial.println((int16_t) 6000-(axm/8) );
    Serial.println((int16_t) 6000-(aym/8) );
    Serial.println((int16_t)( 6000-(int32_t)(azm/8-16384/8)));

    accelgyro.setXAccelOffset((int16_t) 6000-(axm/8) );
    accelgyro.setYAccelOffset((int16_t) 6000-(aym/8) );
    accelgyro.setZAccelOffset((int16_t)( 6000-(int32_t)(azm/8-16384/8)));

    Serial.println("Accelerometer calibration done!");
  }


  void calibrateAccSoft(int cnt, int interval){
    Serial.println("starting software accel calibration dont move mpu!");
    float axm=0, aym=0, azm=0;
    for(int i=0;i<cnt;i++){
      readAcc();
      axm+=(int32_t)ax;
      aym+=(int32_t)ay;
      azm+=(int32_t)az;
      delay(interval);
    }
    axm/=cnt;
    aym/=cnt;
    azm/=cnt;
    Serial.println(-axm);
    Serial.println(-aym);
    Serial.println(16384-azm);
    axo=0-axm;
    ayo=0-aym;
    azo=16384-azm;
    Serial.println("Accel soft calibration done!");
  }

  void calibrateGyroSoft(int cnt, int intervalMili){
    Serial.println("starting gyro soft calibration dont move mpu!");
    float gxm=0, gym=0, gzm=0;
    for(int i=0;i<cnt;i++){
      readAcc();
      gxm+=(int32_t)gx;
      gym+=(int32_t)gy;
      gzm+=(int32_t)gz;
      delay(intervalMili);
    }

    gxm/=cnt;
    gym/=cnt;
    gzm/=cnt;

    Serial.println(-gxm);
    Serial.println(-gym);
    Serial.println(-gzm);
    gxo=-gxm;
    gyo=-gym;
    gzo=-gzm;
    Serial.println("Gyro soft calibration done!");
  }

  void setAcc(){
#ifdef DEBUGGYRO
    //if(!Serial)
      Serial.begin(9600); 
#endif
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();
    Serial.println("Testing device connections...\n");
    
    calibrateGyro(10, 10);
    calibrateGyroSoft(300, 10);
    calibrateAcc(10, 10);
    calibrateAccSoft(300, 10);

    ahrs.begin(10);
  }

}