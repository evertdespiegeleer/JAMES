#include <Wire.h>
#include <L3G.h>
#include <LSM303.h>
#include <Kalman.h>

#define gyroToRasPSFactor 8.75/1000*DEG_TO_RAD
L3G gyro;
LSM303 compass;

unsigned long prevOrientationReading;

float g_zero[3]; //zero values for {gyro.g.x, gyro.g.y, gyro.g.z}

double prevReadHeading;
int fullLoopSumNr;
//double continuous_yaw_real_angle;

double g_angles[3] = {0,0,0}; //{pitch, roll,yaw}
double a_angles[3] = {0,0,0}; //{pitch, roll, continuous_yaw}
double compl_angles[3] = {0,0,0}; //{pitch, roll,yaw} //Complementary Filter
double kalman_angles[3] = {0,0,0}; //Kalman Filter

Kalman kalmanPitch;
Kalman kalmanRoll;

void setupImu() {
  Wire.begin();
  gyro.init();
  compass.init();
  gyro.enableDefault();
  compass.enableDefault();
}

void calibrateG (int cycles) { //Gyro calibration
  float errorSums[3] = {0,0,0};
  for(int i; i<cycles; i++) {
    gyro.read();
    errorSums[0] += gyro.g.x;
    errorSums[1] += gyro.g.y;
    errorSums[2] += gyro.g.z;
    Serial.println("Calibrate...");
    delay(10);
  }
  g_zero[0] = errorSums[0]/cycles;
  g_zero[1] = errorSums[1]/cycles;
  g_zero[2] = errorSums[2]/cycles;
}

void readOrientation() {
  gyro.read();
  compass.read();
  
  unsigned long currentMillis = millis();
  int dt = (currentMillis-prevOrientationReading);
  prevOrientationReading = currentMillis;
  
  g_angles[0] = g_angles[0] + dt * (gyro.g.x - g_zero[0])*gyroToRasPSFactor * 0.001;
  g_angles[1] = g_angles[1] + dt * (gyro.g.y - g_zero[1])*gyroToRasPSFactor * 0.001;
  g_angles[2] = g_angles[2] - dt * (gyro.g.z - g_zero[2])*gyroToRasPSFactor * 0.001;  
  
  a_angles[0] = (double)atan2(-compass.a.x, compass.a.z);
  a_angles[1] = (double)atan2(-compass.a.y, compass.a.z);

  compl_angles[0] = 0.98 * (compl_angles[0] + (gyro.g.x - g_zero[0]) * dt * 0.001 * gyroToRasPSFactor) + 0.02 * a_angles[0];
  compl_angles[1] = 0.98 * (compl_angles[1] + (gyro.g.y - g_zero[1]) * dt * 0.001 * gyroToRasPSFactor) + 0.02 * a_angles[1];


  kalmanPitch.setAngle(a_angles[0]);
  kalmanRoll.setAngle(a_angles[1]);
  kalman_angles[0] = kalmanPitch.getAngle(a_angles[0], (gyro.g.x - g_zero[0])*gyroToRasPSFactor, dt*0.001);
  kalman_angles[1] = kalmanRoll.getAngle(a_angles[1], (gyro.g.y - g_zero[1])*gyroToRasPSFactor, dt*0.001);
  

  if ((compass.heading() - prevReadHeading) >= 300) {
    fullLoopSumNr = fullLoopSumNr - 1;
  }
  else if ((compass.heading() - prevReadHeading) <= -300) {
    fullLoopSumNr = fullLoopSumNr + 1;
  }
  a_angles[2] = (fullLoopSumNr*360 + compass.heading())*DEG_TO_RAD;
  compl_angles[2] = 0.98 * (compl_angles[2] - (gyro.g.z - g_zero[1]) * dt * 0.001 * gyroToRasPSFactor) + 0.02 * a_angles[2];
  prevReadHeading = compass.heading();
}




void setup () {
  Serial.begin(9600);
  setupImu();
  Serial.print("Calibrating...");
  calibrateG(20);
}

void loop() {
  readOrientation();
  //Serial.print("");Serial.print(g_angles[0]*RAD_TO_DEG);
  Serial.print(" ");Serial.print(a_angles[0]*RAD_TO_DEG);
  //Serial.print(" ");Serial.print(compl_angles[0]*RAD_TO_DEG);
  Serial.print(" ");Serial.print(" ");Serial.print(kalman_angles[0]*RAD_TO_DEG);
  Serial.println(" ");
}
