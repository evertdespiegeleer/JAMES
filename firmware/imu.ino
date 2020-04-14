#include <Wire.h>
#include <L3G.h>
#include <LSM303.h>
#include <BMP180I2C.h>

#define gyroToRadsPerSecFactor 8.75/1000*DEG_TO_RAD
L3G gyro;
LSM303 compass;
BMP180I2C bmp180(0x77);

unsigned long prevOrientationReading;

int fullLoopSumNr;
int prevReadHeading;

float g_zero[3]; //zero values for {gyro.g.x, gyro.g.y, gyro.g.z}

double f_angles[3] = {0,0,0}; //{pitch, roll, continuous_yaw} //FILTERED angles, 
float pressure;

void setupImu() {
  Wire.begin();
  gyro.init();
  compass.init();
  bmp180.begin();
  gyro.enableDefault();
  compass.enableDefault();
  bmp180.resetToDefaults();
  bmp180.setSamplingMode(BMP180MI::MODE_UHR); //enable ultra high resolution mode for pressure measurements
}

void calibrateG (int cycles, unsigned int nextState) { //Gyro calibration
  float errorSums[3] = {0,0,0};
  for(int i = 0; i<cycles; i++) {
    sendMsg(976, i);
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
  
  f_angles[0] = 0.98 * (f_angles[0] + (gyro.g.x - g_zero[0]) * dt * 0.001 * gyroToRadsPerSecFactor) + 0.02*atan2(-compass.a.x, compass.a.z);
  f_angles[1] = 0.98 * (f_angles[1] + (gyro.g.y - g_zero[1]) * dt * 0.001 * gyroToRadsPerSecFactor) + 0.02*atan2(-compass.a.y, compass.a.z);


  int heading = compass.heading();
  if(heading<180) {
    heading=heading+180;
  }
  else {
    heading=heading-180;
  }
  
  if ((heading - prevReadHeading) >= 300) {
    fullLoopSumNr = fullLoopSumNr - 1;
  }
  else if ((heading - prevReadHeading) <= -300) {
    fullLoopSumNr = fullLoopSumNr + 1;
  }
  f_angles[2] = 0.98 * (f_angles[2] - (gyro.g.z - g_zero[1]) * dt * 0.001 * gyroToRadsPerSecFactor) + 0.02 * (fullLoopSumNr * 360 + heading) * DEG_TO_RAD;
  prevReadHeading = heading;

  
}

void readAirPressure () {
  if(bmp180.measurePressure() && bmp180.hasValue()) {
    pressure = bmp180.getPressure();
  }
}

void sendOrientation () {
  sendMsg(600, (int)(getImuAngle(0)*1000+7000));
  sendMsg(601, (int)(getImuAngle(1)*1000+7000));
  sendMsg(602, (int)((getImuAngle(2) - fullLoopSumNr * 360 * DEG_TO_RAD)*1000+7000));
}

void sendPressure () {
  sendMsg(603, (int)(pressure-100000));
}

double getImuAngle(int index) {
  return f_angles[index];
}

boolean checkMaxAngleReached () {
  if ((abs(getImuAngle(0)) >= maxAbsoluteAngle) || (abs(getImuAngle(1)) >= maxAbsoluteAngle)) {
    return true;
  }
  else {
int getFullLoopSumNr () {
  return fullLoopSumNr;
}
