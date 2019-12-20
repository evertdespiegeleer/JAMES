void sign_setup () {
  pinMode(signLed1, OUTPUT);
  pinMode(signLed3, OUTPUT);
  pinMode(signLed9, OUTPUT);
  sign_set();
}

int factor9;
int factor3;
int factor1;

unsigned long blinkTreshHoldStamp = blinkPeriod;
boolean blinkValue;

void sign_set () {
  factor9 = (int)(floor)(state/9);
  factor3 = (int)(floor)((state-factor9*9)/3);
  factor1 = (int)(floor)((state-factor9*9-factor3*3)/1);
}

void sign_loop () {
  if(millis() >= blinkTreshHoldStamp) { //Defines blink time
    blinkTreshHoldStamp = millis() + blinkPeriod;
  }
  blinkValue = (millis() > blinkTreshHoldStamp-blinkPeriod/2);
  
  if (factor9 == 0) {
    digitalWrite(signLed9, LOW);
  }
  else if (factor9 == 1) {
    digitalWrite(signLed9, HIGH);
  }
  else {
    digitalWrite(signLed9, blinkValue);
  }

  if (factor3 == 0) {
    digitalWrite(signLed3, LOW);
  }
  else if (factor3 == 1) {
    digitalWrite(signLed3, HIGH);
  }
  else {
    digitalWrite(signLed3, blinkValue);
  }

  if (factor1 == 0) {
    digitalWrite(signLed1, LOW);
  }
  else if (factor1 == 1) {
    digitalWrite(signLed1, HIGH);
  }
  else {
    digitalWrite(signLed1, blinkValue);
  }
}
