unsigned int prevState = 0;
void setState(int stateNr) {
  prevState = state;
  state = stateNr;
  sign_set();
  sendMsg(10, stateNr);
}

void stateLoop() {
  armed = false; //Armed should always be set false, unless a state, i.e. 10, explicitly states it should not be.

  if (rxCmd() == 11) {
    setState((int)rxArg());
    rxFlush();
  }
  
  
  if (state == 0) { //Boot
    if((readBattery() > 4) && false) {
      setState(19);
    }
    else {
      setupImu();
      setState(1);
    }
  }
  
  else if (state == 1) { //Programming ESCs
    if (programESCs()) {
      setState(2);
    }
  }

  else if (state == 2) { //Wait for main computer wakeup message
    if (rxCmd() == 1) {
       setState(3);
    }
  }

  else if (state == 3) { //Check 2-way communication
    if(twoWayComCheck()) {
      setState(4);
    }
  }

  else if (state == 4) { //Wait for PID-values
    pidReadInParams();
    if(pidValuesSet()) {
      setState(5);
      pidsStartup();
    }
  }

  else if (state == 5) {
    
  }

  else if (state == 6) { //PID values and setpoints welcome
    armed = false;
    readOrientation();
    readAirPressure();
    pidReadInParams();
    sendOrientation();
    sendPressure();
    sendBattery();
    readAdditionalThrust();
    updateESCs();
  }

  else if (state == 7) { //Calibration of the gyros
    calibrateG(200, prevState);
  }

  else if (state == 8) {
    sendMsg(920, (int)(readBattery()*100)); //Send battery voltage times 100
    setState(prevState);
  }

  else if (state == 10) { //USUAL FLIGHT MODE, ENGINES RUNNING
    armed = true;
    if(tetheredMode && armed) { //If in tethered mode, serial communication happens over a cable. If the last message received over serial is more than 2 sec. ago, the serial connection is considered broken. unarm.
      if (millis() >= (lastReceivedSerialMsgs + 2000)) {
        armed = false;
        setState(22);
      } 
    }
    readOrientation();
    if(tetheredMode && armed) { //If in tethered mode and exceeding the absolute maximum angle on the pitch or roll axis, disarm and go to safety mode 23.
      if(checkMaxAngleReached()) {
        armed = false;
        setState(23);
      }
    }
    readAirPressure();
    pidReadInParams();
    sendOrientation();
    sendPressure();
    sendBattery();
    readAdditionalThrust();
    pidLoop();
    updateESCs();
  }

  else {
    updateESCs();
  }
}
