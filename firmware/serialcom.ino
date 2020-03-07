void serial_setup () {
  Serial.begin(57600);
  Serial.setTimeout(serialTimeout);
}

char serialInBuf[24];
char standardizedMsg[24] = {'g',';','0','0','0',';','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0',';','e',';'};
unsigned int cmd = 0;
float arg = 0;

void rxLoop () {
  bool standardizedFound = false;

  while((!standardizedFound) && (Serial.available() > 0)) {

    // Shift entire buffer to the left and add new char
      for(int i=0;i<23;i++) {
        serialInBuf[i] = serialInBuf[i+1];
      }
      serialInBuf[23] = Serial.read();
    // !Shift entire buffer to the left and add new char

    // Check for standardized message in buffer
    standardizedFound = true;
      for(int i=0;i<2;i++){
        if(serialInBuf[i] != standardizedMsg[i]) {
          standardizedFound = false;
        }
      }
      for(int i=22;i<24;i++){
        if(serialInBuf[i] != standardizedMsg[i]) {
          standardizedFound = false;
        }
      }
      // !Check for standardized message in buffer
  }


      if (standardizedFound) { //Standardized msg in serialBufReadIndex.
        //read the cmd bytes into cmd
        cmd = (serialInBuf[2]-'0')*100+(serialInBuf[3]-'0')*10+(serialInBuf[4]-'0');
        char argBuffer [15];
        for(int i = 6; i < 21; i++) { //read the argument bytes into arg
          argBuffer[i-6] = serialInBuf[i];
        }
        arg = atoi(argBuffer);
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

void sendMsg(int cmd, int arg) {
  bool argsent = false;
  for(int i=0; i<24; i++) {
    if(i<2) { //writing START
      Serial.print(standardizedMsg[i]);
    }
    else if (i == 2) { //Writing CMD
      if(cmd<100) {
        Serial.print('0');
      }
      if(cmd<10) {
        Serial.print('0');
      }
      Serial.print(cmd,DEC);
      Serial.print(';');
    }
    else if ((i > 5) && (i < 21)) { //Writing arguments
      if(arg<pow(10,15-(i-5))) {
        Serial.print('0');
      }
      else {
        if(!argsent) {
          Serial.print(arg,DEC);
          argsent = true;
        }
      }
    }
    else if (i>20) {
      Serial.print(standardizedMsg[i]);
    }
  }
}
