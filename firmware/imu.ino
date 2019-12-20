#include <Wire.h>
#include <L3G.h>
#include <LSM303.h>

#define gyroToRasPSFactor 8.75/1000*DEG_TO_RAD
L3G gyro;
LSM303 compass;

unsigned long prevOrientationReading;

float g_zero[3]; //zero values for {gyro.g.x, gyro.g.y, gyro.g.z}

double f_angles[3] = {0,0,0}; //{pitch, roll,yaw} //FILTERED angles

void setupImu() {
  Wire.begin();
  gyro.init();
  compass.init();
  gyro.enableDefault();
  compass.enableDefault();
}

void calibrateG (int cycles, unsigned int nextState) { //Gyro calibration
  float errorSums[3] = {0,0,0};
  for(int i; i<cycles; i++) {
    gyro.read();
    errorSums[0] += gyro.g.x;
    errorSums[1] += gyro.g.y;
    errorSums[2] += gyro.g.z;
    delay(10);
  }
  g_zero[0] = errorSums[0]/cycles;
  g_zero[1] = errorSums[1]/cycles;
  g_zero[2] = errorSums[2]/cycles;
  setState(nextState);
}

void readOrientation() {
  gyro.read();
  compass.read();
  
  unsigned long currentMillis = millis();
  int dt = (currentMillis-prevOrientationReading);
  prevOrientationReading = currentMillis;
  
  f_angles[0] = 0.98 * (f_angles[0] + (gyro.g.x - g_zero[0]) * dt * 0.001 * gyroToRasPSFactor) + 0.02*atan2(-compass.a.x, compass.a.z);
  f_angles[1] = 0.98 * (f_angles[1] + (gyro.g.y - g_zero[1]) * dt * 0.001 * gyroToRasPSFactor) + 0.02*atan2(-compass.a.y, compass.a.z);
  f_angles[2] = 0.98 * (f_angles[2] - (gyro.g.z - g_zero[1]) * dt * 0.001 * gyroToRasPSFactor) + 0.02*compass.heading()*DEG_TO_RAD;
}

void sendOrientation () {
  sendMsg(600, (int)(f_angles[0]*1000+7000));
  sendMsg(601, (int)(f_angles[1]*1000+7000));
  sendMsg(602, (int)(f_angles[2]*1000+7000));
}

double getImuAngle(int index) {
  return f_angles[index];
}

