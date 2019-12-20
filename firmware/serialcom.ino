void serial_setup () {
  Serial.begin(57600);
  Serial.setTimeout(50);
}

char serialInBuf[24];
char standardizedMsg[24] = {'g',';','0','0','0',';','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0',';','e',';'};
unsigned int cmd = 0;
unsigned int arg = 0;

void rxLoop () {
  if (Serial.available() > 0) {
    //Place every byte one place forward in the register
    for(int i=0;i<23;i++) {
        serialInBuf[i] = serialInBuf[i+1]; //move all element to the left except first one
    }
    //Add read byte as last byte in the register
    serialInBuf[23] = Serial.read();

    
      //Check if a standardized message is inside the register
      boolean standardizedFound = true;
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

      
      
      if (standardizedFound) { //Standardized msg in serialBufReadIndex.
        //read the cmd bytes into cmd
        cmd = (serialInBuf[2]-'0')*100+(serialInBuf[3]-'0')*10+(serialInBuf[4]-'0');
        char argBuffer [15];
        for(int i = 6; i < 21; i++) { //read the argument bytes into arg
          argBuffer[i-6] = serialInBuf[i];
        }
        arg = atoi(argBuffer);
      }
      else {
        cmd = 0; //CMD = 0 means no standardized message inside register.
        arg = 0;
      }

      
  }
}

boolean rxAvailable () {
  return (cmd != 0);
}

unsigned int rxCmd () {
  return cmd;
}

unsigned int rxArg () {
  return arg;
}

void rxFlush() {
  cmd = 0;
  arg = 0;
}

void sendMsg(int cmd, int arg) {
  boolean argsent = false;
  char argBuffer[15];
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

