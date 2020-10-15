#define esc1 9
#define esc2 6
#define esc3 5
#define esc4 3
#define disArmedMicroSeconds 500
#define minMicroSeconds 800
#define minMicroSecondsMotorsTurning 870
#define maxMicroSeconds 1800
#define signLed1 A2 //Corresponds to L7
#define signLed3 A3 //Corresponds to L8
#define signLed9 A1 //Corresponds to L6

#include <Servo.h>
bool armed;

Servo escs[4];

int thrustPartitions[5]; //{altitude, pitch, roll, yaw, additional_thrust}

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
void updateESCs () {
  if(armed) {
    //TODO: ADD YAW
    int escValues[4] = {
    minMicroSeconds + thrustPartitions[0] + thrustPartitions[4] + thrustPartitions[1] + thrustPartitions[2] - thrustPartitions[3],
    minMicroSeconds + thrustPartitions[0] + thrustPartitions[4] + thrustPartitions[1] - thrustPartitions[2] + thrustPartitions[3],
    minMicroSeconds + thrustPartitions[0] + thrustPartitions[4] - thrustPartitions[1] - thrustPartitions[2] - thrustPartitions[3],
    minMicroSeconds + thrustPartitions[0] + thrustPartitions[4] - thrustPartitions[1] + thrustPartitions[2] + thrustPartitions[3]
    };
    

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

void setup() {
  programESCs();
  pinMode(signLed1, OUTPUT);
  pinMode(signLed3, OUTPUT);
  pinMode(signLed9, OUTPUT);
}

unsigned long prevMillis = 0;

void loop() {
  if(programESCs()){
    unsigned long dt = millis() - prevMillis;
    if(dt <= 2000) {
      armed = false;
      digitalWrite(signLed9, LOW);
    }
    if(dt > 4000) {
      armed = true;
      digitalWrite(signLed9, HIGH);
    }
    if(dt > 6000) {
      prevMillis = millis();
    }
    updateESCs();
  }
}
