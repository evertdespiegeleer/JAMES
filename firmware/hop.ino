unsigned int hopThrust = 0;
unsigned int hopDuration = 0;
unsigned int stateAfterHop = 0;

bool hopping = false;
unsigned int hopStart = 0;

void hopLoop () {
  armed = false;
  if(rxAvailable()) {
    if(rxCmd() == 210) {
      hopDuration = rxArg();
      rxFlush();
    }
    else if(rxCmd() == 211) {
      stateAfterHop = rxArg();
      rxFlush();
    }
    else if(rxCmd() == 212) {
      hopThrust = rxArg();
      rxFlush();
    }
    else if(rxCmd() == 213) {
      if(!hopping) { //If a hop is not happening yet
      if(!armed) { // Hop is a transition state, usually from standing on the ground to stable flight. It is used to get the vehicle away from the earth as fast as possible, so guidance can properly do its thing. Therefore, it should not be performed while already armed. Hop IS an arming manoeuvre.
        armed = true;
        rxFlush();
        hopping = true;
        hopStart = millis();
        setThrustPartition(0, 0);
        setThrustPartition(1, 0);
        setThrustPartition(2, 0);
        setThrustPartition(3, 0);
        setThrustPartition(4, 1);
        updateESCs(); //Spin up the engines
      }
      else { //If hop was commanded tried in a armed state, send to a safety error mode.
        setState(24);
      }
    }
    else if (millis() >= hopStart + hopEngineSpinupTime + hopDuration) { //Engines are spinning, hop manoeuvre ending.
      hopStart = 0;
      hopping = false;
      setState(stateAfterHop);
    }
    else if (millis() >= hopStart + hopEngineSpinupTime) { //Engines are spinning, set hop thrust
      armed = true;
      setThrustPartition(4, hopThrust);
      updateESCs();
    }
    }
  }
}
