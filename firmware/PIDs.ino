#include <PID_v1.h>

// Main array: {altitude, pitch, roll, yaw}
double pidCsts [4][3]; // Child array: {Kp, Ki, Kd}
double pidVars [4][3]; // Child array: {setpoint, input, output}

//Specify the links and initial tuning parameters
PID altitudeController(&pidVars[0][1], &pidVars[0][2], &pidVars[0][0], pidCsts[0][0], pidCsts[0][1], pidCsts[0][2], DIRECT);
PID pitchController(&pidVars[1][1], &pidVars[1][2], &pidVars[1][0], pidCsts[1][0], pidCsts[1][1], pidCsts[1][2], DIRECT);
PID rollController(&pidVars[2][1], &pidVars[2][2], &pidVars[2][0], pidCsts[2][0], pidCsts[2][1], pidCsts[2][2], DIRECT);
PID yawController(&pidVars[3][1], &pidVars[3][2], &pidVars[3][0], pidCsts[3][0], pidCsts[3][1], pidCsts[3][2], DIRECT);
PID pids [4] = {altitudeController, pitchController, rollController, yawController};

boolean pidCstsReceived[4][3];
boolean pidValuesSet () {
  boolean ok = true;
  for(int r = 0; r < 4; r++) {
    for(int i = 0; i < 3; i++) {
      ok=ok*pidCstsReceived[r][i];
    }
  }
  return ok;
}

void pidReadInParams () {
  if(rxAvailable()) {
    char buf [4];
    sprintf (buf, "%03i", rxCmd());
    if((rxCmd() > 99) && (rxCmd() < 133)) { //PID parameter value received over comline. Argument is 1000 times wanted parameter.
      int action = buf [2] - '0';
      int pidSelection = buf [1] - '0';
      if(action == 0) {
        pidCsts[pidSelection][action] = (double)(rxArg());
      }
      else {
       pidCsts[pidSelection][action] = (double)(rxArg()/100); 
      }
      pidCstsReceived[pidSelection][action] = true;
      // sendMsg(900+10*pidSelection+action, (int)pidCsts[pidSelection][action]);///////
      pids[pidSelection].SetTunings(pidCsts[pidSelection][0], pidCsts[pidSelection][1], pidCsts[pidSelection][2]);
      rxFlush();
    }
    else if ((rxCmd() > 140) && (rxCmd() < 145)) { //Setpoints received over comline (141 -> 144).
      int pidSelection = (buf [2] - '0');
      pidVars[pidSelection-1][0] = (double)((rxArg()-3142)/1000);
      sendMsg(930+pidSelection-1, (int)(pidVars[pidSelection-1][0])*1000); //Send setpoint*1000 back over serial for debugging, 930 = alti, 931 = pitch...
      rxFlush();
    }
  }
}

void pidsStartup () {
  for(int i = 0; i<4; i++) { //Turn on all pids
    pids[i].SetOutputLimits(-maxMicroSeconds,maxMicroSeconds);
    pids[i].SetSampleTime(pidSampleTime);
    pids[i].SetMode(AUTOMATIC);
  }
}

void pidLoop () {
  if(armed) {

  //TODO: add altitude, add YAW
  pidVars[0][1] = 0; //Input altidude
  pidVars[1][1] = getImuAngle(0); //Input pitch
  pidVars[2][1] = getImuAngle(1); //Input roll

  pidVars[3][1] = 0; //Set Yaw offset setpoint to 0

  for(int i = 0; i<4; i++) { //Compute all PID output values
    pids[i].Compute();
  }

  setThrustPartition(0, pidVars[0][2]);
  setThrustPartition(1, pidVars[1][2]);
  setThrustPartition(2, pidVars[2][2]);

  }
}
