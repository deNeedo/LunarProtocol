#include <RH_ASK.h>
#include <SPI.h>

RH_ASK driver(2000, 11, 12, 10);

unsigned long counter = 0;  // will increase every send

void setup() {
  Serial.begin(9600);
  if (!driver.init()) {
    Serial.println("RF init failed");
    while (1) {}
  }
  Serial.println("Transmitter ready");
}

void loop() {
  char msg[16];                       // buffer for the text
  snprintf(msg, sizeof(msg), "%lu", counter);  // convert counter to ASCII

  driver.send((uint8_t *)msg, strlen(msg));
  driver.waitPacketSent();

  Serial.print("Sent: ");
  Serial.println(msg);

  counter++;      // increment for next loop
  delay(1000);    // send once per second
}