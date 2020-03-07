#include "defs.h"

unsigned int state;
bool armed;

void setup() {
  state = 0;
  sign_setup();
  serial_setup();
}

void loop() {
  rxLoop();
  stateLoop();
  sign_loop();
}
