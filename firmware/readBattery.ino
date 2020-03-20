double readBattery () {
  return (analogRead(battery)*0.021);
}

void sendBattery () {
  sendMsg(920, (int)(readBattery()*100));
}
