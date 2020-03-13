unsigned int comCheckSentTime = 0;

boolean twoWayComCheck () {
  if(comCheckSentTime == 0) {
    //Check has not been sent
    comCheckSentTime = (unsigned int)(millis()/100);
    sendMsg(2, comCheckSentTime);
  }
  else {
    if(((int)millis() - comCheckSentTime) < 300) { //Message has to be returned in max 300ms
      if((rxCmd() == 3) && ((int)rxArg() == comCheckSentTime)) {
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
