// Libraries
#include "TestSuite.h"

void setup() {
  // Open serial port
  Serial.begin(115200);
  // Setup radio
  initRadio();
}

void loop() {
  // measureDistance();
  pingDevice();
}