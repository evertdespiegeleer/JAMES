unsigned long lastReceivedSerialMsgs = 0;

void serial_setup () {
  Serial.begin(57600);
  Serial.setTimeout(serialTimeout);
}

char standardizedMsg[14] = {'g',';','0','0','0',';','0','0','0','0','0',';','e',';'};
unsigned int cmd = 0;
float arg = 0;

#define cmdMsgLength 3
#define argMsgLength 5
#define totalFormatedMsgLength (2+cmdMsgLength+1+argMsgLength+3)
#define msgFormater "g;%03d;%05d;e;"

char serialInBuf[totalFormatedMsgLength];

void rxLoop () {
  bool standardizedFound = false;

  while((!standardizedFound) && (Serial.available() > 0)) {

    // Shift entire buffer to the left and add new char
      for(int i=0;i<totalFormatedMsgLength-1;i++) {
        serialInBuf[i] = serialInBuf[i+1];
      }
      serialInBuf[totalFormatedMsgLength-1] = Serial.read();
    // !Shift entire buffer to the left and add new char

    // Check for standardized message in buffer
    standardizedFound = true;
      for(int i=0;i<2;i++){
        if(serialInBuf[i] != standardizedMsg[i]) {
          standardizedFound = false;
        }
      }
      for(int i=totalFormatedMsgLength-2;i<totalFormatedMsgLength;i++){
        if(serialInBuf[i] != standardizedMsg[i]) {
          standardizedFound = false;
        }
      }
      // !Check for standardized message in buffer
  }
  


      if (standardizedFound) { //Standardized msg in serialBufReadIndex.
        //read the cmd bytes into cmd
        cmd = (serialInBuf[2]-'0')*100+(serialInBuf[3]-'0')*10+(serialInBuf[4]-'0');
        char argBuffer [argMsgLength];
        for(int i = 2+1+cmdMsgLength; i < 2+1+cmdMsgLength+argMsgLength; i++) { //read the argument bytes into arg
          argBuffer[i-6] = serialInBuf[i];
        }
        arg = atoi(argBuffer);
        lastReceivedSerialMsgs = millis();
        if(rxArg () == 777) rxFlush(); // 777 is a heartbeat message with the sole purpose of updating lastReceivedSerialMsgs.
        // sendMsg(950, arg);
      }
}

bool rxAvailable () {
  return (cmd != 0);
}

unsigned int rxCmd () {
  return cmd;
}

float rxArg () {
  return arg;
}

void rxFlush() {
  cmd = 0;
  arg = 0;
}

unsigned int lastPingCycles = 0;
void pingLoop() {
  if(lastPingCycles >= 2) {
    sendMsg(683,0);
    lastPingCycles = 0;
  } else {
    lastPingCycles++;
  }
}

char msgToSendFormated[totalFormatedMsgLength+1];
void sendMsg(int _cmd, int _arg) {
 // sprintf(msgToSendFormated, msgFormater, _cmd, _arg);
 // Serial.print(msgToSendFormated);
 Serial.print("g;");Serial.print(_cmd);Serial.print(';');Serial.print(_arg);Serial.print(";e;");
}
