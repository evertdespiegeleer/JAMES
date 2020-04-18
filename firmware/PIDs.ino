// PID class
class PID {
  private:
    float error;
    float cumError;
    float errorRate;

    float setpoint;
    float fbInput;
    float output;
    float Kp;
    float Ki;
    float Kd;

    unsigned long previousTime;
    float prevError;

    float outputMax;
    float outputMin;

    float sampleTime;

  public:
    PID (float setpointRef, float fbInputRef, float KpCst, float KiCst, float KdCst) {
      setpoint = setpointRef;
      fbInput = fbInputRef;
      Kp = KpCst;
      Ki = KiCst;
      Kd = KdCst;
      previousTime = millis();
    }

    void Compute () {
      unsigned long currentTime = millis();
      float elapsedTime = (currentTime - previousTime) * 0.001;
        if (elapsedTime >= sampleTime) {
        error = setpoint - fbInput;
        cumError += error * elapsedTime;
        errorRate = (error - prevError)/elapsedTime;

        float tempOutput = Kp * error + Ki * cumError + Kd * errorRate;
        output = max(outputMin, min(outputMax, tempOutput));

        prevError = error;
        previousTime = currentTime;
      }
    }

    void SetTunings (float KpCst, float KiCst, float KdCst) {
      Kp = KpCst;
      Ki = KiCst;
      Kd = KdCst;
    }

    void SetOutputLimits (float min, float max) {
      outputMin = min;
      outputMax = max;
    }

    void SetSampleTime (unsigned long time) {
      sampleTime = time * 0.001;
    }

    void resetWindup () {
      cumError = 0;
    }

    void resetAll () {
      output = 0;
      cumError = 0;
      error = 0;
    }

    void setSetpoint (float sp) {
      setpoint = sp;
    }

    void setFbInput (float fbInput_) {
      fbInput = fbInput_;
    }

    float getOutput () {
      return output;
    }
};
// ! PID class

// Main array: {altitude, pitch, roll, yaw}
float pidCsts [4][3]; // Child array: {Kp, Ki, Kd}
float pidVars [4][3]; // Child array: {setpoint, input, output}

//Specify the links and initial tuning parameters
PID altitudeController = PID(pidVars[0][1], pidVars[0][0], pidCsts[0][0], pidCsts[0][1], pidCsts[0][2]);
PID pitchController = PID(pidVars[1][1], pidVars[1][0], pidCsts[1][0], pidCsts[1][1], pidCsts[1][2]);
PID rollController = PID(pidVars[2][1], pidVars[2][0], pidCsts[2][0], pidCsts[2][1], pidCsts[2][2]);
PID yawController = PID(pidVars[3][1], pidVars[3][0], pidCsts[3][0], pidCsts[3][1], pidCsts[3][2]);
PID pids [4] = {altitudeController, pitchController, rollController, yawController};

double desiredYawNormalized;

bool pidCstsReceived[4][3];
bool pidValuesSet () {
  bool ok = true;
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
    if((rxCmd() > 99) && (rxCmd() < 133)) { //PID parameter value received over comline.
      int action = buf [2] - '0';
      int pidSelection = buf [1] - '0';
      if(action == 0) {
        pidCsts[pidSelection][action] = (rxArg());
      }
      else {
       pidCsts[pidSelection][action] = (rxArg()/100); 
      }
      pidCstsReceived[pidSelection][action] = true;
      // sendMsg(900+10*pidSelection+action, (int)pidCsts[pidSelection][action]);///////
      pids[pidSelection].SetTunings(pidCsts[pidSelection][0], pidCsts[pidSelection][1], pidCsts[pidSelection][2]);
      rxFlush();
    }
    else if ((rxCmd() > 140) && (rxCmd() < 145)) { //Setpoints received over comline (141 -> 144).
      int pidSelection = (buf [2] - '0');
      
      if(pidSelection == 4) { //Yaw, heading
        desiredYawNormalized = (double)((rxArg()-3142)/1000);
      }
      else {
        pidVars[pidSelection-1][0] = (double)((rxArg()-3142)/1000);
        pids[pidSelection-1].setSetpoint(pidVars[pidSelection-1][0]);
      }
      rxFlush();
    }
  }
}

void pidsStartup () {
  for(int i = 0; i<4; i++) { //Turn on all pids
    pids[i].SetOutputLimits(-(maxMicroSeconds - minMicroSeconds) * 0.5, (maxMicroSeconds - minMicroSeconds) * 0.5);
    pids[i].SetSampleTime(pidSampleTime);
  }
}

void resetWindup () {
  for(int i = 0; i<4; i++) {
    pids[i].resetWindup();
  }
}

void pidLoop () {
  //TODO: add altitude
  pidVars[0][1] = 0;pids[0].setFbInput(pidVars[0][1]); //Input altidude
  pidVars[1][1] = getImuAngle(0);pids[1].setFbInput(pidVars[1][1]); //Input pitch
  pidVars[2][1] = getImuAngle(1);pids[2].setFbInput(pidVars[2][1]); //Input roll
  pidVars[3][1] = getImuAngle(2);pids[3].setFbInput(pidVars[3][1]); //Input yaw

  //Find shortest yaw
  pidVars[3][0] = (double)(desiredYawNormalized + ((getFullLoopSumNr() + 2) * 360 * DEG_TO_RAD));
  while(abs(pidVars[3][0] - (360 * DEG_TO_RAD) - pidVars[3][1]) < abs(pidVars[3][0] - pidVars[3][1])) {
    pidVars[3][0] = pidVars[3][0] - (360 * DEG_TO_RAD);
  }
  pids[3].setSetpoint(pidVars[3][0]);


  for(int i = 0; i<4; i++) { //Compute all PID output values
    pids[i].Compute();
    sendMsg(610 + i, (int)(pids[i].getOutput() + 5000));
  }

  setThrustPartition(0, pids[0].getOutput());
  setThrustPartition(1, pids[1].getOutput());
  setThrustPartition(2, pids[2].getOutput());
  setThrustPartition(3, pids[3].getOutput());
}
