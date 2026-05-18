// Libraries
#include "Globals.h"
#include "RadioManager.h"

SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY);

volatile bool receivedFlag = false;

void setFlag() {
  receivedFlag = true;
}

void initRadio() {
  int state = radio.begin(868.6, 500.0, 8, 8);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("Radio OK");
    pinMode(LORA_LED, OUTPUT);
    // radio.setDio1Action(setFlag);
    // radio.startReceive();
  }
}

bool channelFree() {
  float rssi = radio.getRSSI();
  Serial.print("RSSI: ");
  Serial.println(rssi);
  return (rssi < -105); // more realistic threshold
}

bool sendPacket(Packet* pkt) {
  // for (int i = 0; i < 3; i++) {
  // delay(DEVICE_ID * 1000);
  // if (!channelFree()) {delay(random(5, 25)); continue;}
  int state = radio.transmit((uint8_t*)pkt, sizeof(Packet));
  if (state == RADIOLIB_ERR_NONE) {blink(); return true;}
  return false;
}

bool receivePacket(Packet* pkt) {
  // if (!receivedFlag) return false;
  // receivedFlag = false;
  int state = radio.receive((uint8_t*)pkt, sizeof(Packet));
  // radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    if (pkt->sender == DEVICE_ID) return false; /*ignore own*/
    if (pkt->receiver != DEVICE_ID && pkt->receiver != 255) return false; /*ignore not for me*/
    blink(); return true;
  }
  return false;
}