#include <Servo.h>

Servo escs[4];

int thrustPartitions[5]; //{altitude, pitch, roll, yaw, additional_thrust}
int escValues[4];

unsigned long programmingStart = 0;
boolean escs_programmed = false;

boolean programESCs () {
  if(programmingStart == 0) { //Send high value
  escs[0].attach(esc1);
  escs[1].attach(esc2);
  escs[2].attach(esc3);
  escs[3].attach(esc4);
  for(int i=0; i<4; i++) {
    escs[i].writeMicroseconds(maxMicroSeconds);
  }
  programmingStart = millis();
  }
  else if (((programmingStart + 5000) < millis()) && !escs_programmed) { //Wait 5 seconds before sending low value
    for(int i=0; i<4; i++) {
      escs[i].writeMicroseconds(minMicroSeconds);
    }
    escs_programmed = true;
  }
  else if (((programmingStart + 8000) < millis()) && escs_programmed) { //Wait another 8 seconds before moving on
    return true;
  }
  return false;
}

void readAdditionalThrust () {
  if((rxCmd() == 150)) { //If additional_thrust value received over comline
    setThrustPartition(4, rxArg());
    rxFlush();
  }
}

void setThrustPartition(int index, int value) {
  thrustPartitions[index] = value;
}

void updateESCs () {
  if(armed) {
    //TODO: ADD YAW
    escValues[0] = minMicroSeconds + thrustPartitions[0] + thrustPartitions[4] + thrustPartitions[1] + thrustPartitions[2] - thrustPartitions[3];
    escValues[1] = minMicroSeconds + thrustPartitions[0] + thrustPartitions[4] + thrustPartitions[1] - thrustPartitions[2] + thrustPartitions[3];
    escValues[2] = minMicroSeconds + thrustPartitions[0] + thrustPartitions[4] - thrustPartitions[1] - thrustPartitions[2] - thrustPartitions[3];
    escValues[3] = minMicroSeconds + thrustPartitions[0] + thrustPartitions[4] - thrustPartitions[1] + thrustPartitions[2] + thrustPartitions[3];
    

    for(int i=0; i<4; i++) {
      escValues[i] = min(maxMicroSeconds, max(minMicroSecondsMotorsTurning, escValues[i]));
      /*if(tetheredMode) {
        if(thrustPartitions[4] <= 0) {
          escValues[i] = minMicroSeconds;
        }
      }*/
      escs[i].writeMicroseconds(escValues[i]);
    }
  }
  else {
    for(int i=0; i<4; i++) {
      escs[i].writeMicroseconds(minMicroSeconds);
    }
  }
}

void sendESCVals() {
  for(int i=0; i<4; i++) {
      if (armed) {
        sendMsg(620 + i, escValues[i]-minMicroSeconds);
      }
      else {
        sendMsg(620 + i, 0);
      }
  }
}
