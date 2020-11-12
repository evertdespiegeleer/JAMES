double readBattery () {
  // return (analogRead(battery)*analogReadingVoltageFactor); //Temp. disabled battery report
  return 0;
}

void sendBattery () {
  // sendMsg(920, (int)(readBattery()*100)); //Temp. disabled battery report
}
