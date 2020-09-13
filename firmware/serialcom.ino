unsigned long lastReceivedSerialMsgs = 0;

void serial_setup () {
  Serial.begin(57600);
  Serial.setTimeout(serialTimeout);
}

char serialInBuf[14];
char standardizedMsg[14] = {'g',';','0','0','0',';','0','0','0','0','0',';','e',';'};
unsigned int cmd = 0;
float arg = 0;

#define cmdMsgLength 3
#define argMsgLength 5
#define totalFormatedMsgLength (2+cmdMsgLength+1+argMsgLength+3)
#define msgFormater "g;%03d;%05d;e;"

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
      /*for(int i=22;i<24;i++){
        if(serialInBuf[i] != standardizedMsg[i]) {
          standardizedFound = false;
        }
      }*/
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

char msgToSendFormated[totalFormatedMsgLength+1];
void sendMsg(int _cmd, int _arg) {
 sprintf(msgToSendFormated, msgFormater, _cmd, _arg);
 Serial.print(msgToSendFormated);
}
