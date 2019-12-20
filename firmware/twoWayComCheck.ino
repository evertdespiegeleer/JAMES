unsigned int comCheckSentTime = 0;

boolean twoWayComCheck () {
  if(comCheckSentTime == 0) {
    //Check has not been sent
    comCheckSentTime = (int)millis();
    sendMsg(2, comCheckSentTime);
  }
  else {
    if(((int)millis() - comCheckSentTime) < 100) { //Message has to be returned in max 100ms
      if((rxCmd() == 3) && (rxArg() == comCheckSentTime)) {
        //Com operation successfully
        return true;
      } 
    }
    else {
      //Msg took too long to return
      setState(21);
    }
  }
  return false;
}

