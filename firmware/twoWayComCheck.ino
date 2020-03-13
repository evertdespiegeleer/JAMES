unsigned int comCheckSentTime = 0;
unsigned long comCheckSentTimeLong = 0;

boolean twoWayComCheck () {
  if(comCheckSentTime == 0) {
    //Check has not been sent
    comCheckSentTimeLong = (millis());
    comCheckSentTime = (unsigned int)(comCheckSentTimeLong/100);
    sendMsg(2, comCheckSentTime);
  }
  else {
    if((millis() - comCheckSentTimeLong) < 300) { //Message has to be returned in max 300ms
      if((rxCmd() == 3) && ((unsigned int)rxArg() == comCheckSentTime)) {
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
