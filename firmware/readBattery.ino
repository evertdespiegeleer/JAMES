double readBattery () {
  return (analogRead(battery)*analogReadingVoltageFactor);
}

void sendBattery () {
  sendMsg(920, (int)(readBattery()*100));
}
