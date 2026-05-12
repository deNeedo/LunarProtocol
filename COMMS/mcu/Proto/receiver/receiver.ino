#include <RH_ASK.h>
#include <SPI.h>

RH_ASK driver(2000, 11, 12, 10);

void setup() {
  Serial.begin(9600);
  if (!driver.init()) {
    Serial.println("RF init failed");
    while (1) {}
  }
  Serial.println("Receiver ready");
}

void loop() {
  uint8_t buf[16];
  uint8_t buflen = sizeof(buf);

  if (driver.recv(buf, &buflen)) {
    delay(10);
    buf[buflen] = '\0';  // null-terminate

    Serial.print("Received: ");
    Serial.println((char *)buf);
  }
}