// Libraries
#include "TestSuite.h"

void setup() {
  // Open serial port
  Serial.begin(115200);
  // Setup radio
  initRadio();
  uint8_t n1 = 140;
  uint8_t n2 = 180;
  uint8_t n3 = abs(n1 - n2);
  Serial.println(n3);
}

void loop() {
  // measureDistance();
  // pingDevice();
  // clockSync();
}