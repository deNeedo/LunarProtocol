// Libraries
#include "TestSuite.h"

bool flag = false;

void setup() {
  // Open serial port
  Serial.begin(115200);
  // Setup radio
  initRadio();
}

void loop() {
  networkDiagnostics();
  // diagnoseDevice(0, 1);
}