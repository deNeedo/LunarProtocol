// Libraries
#include "Globals.h"
#include "RadioManager.h"

SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY);

volatile bool receivedFlag = false;

void setFlag() {
  receivedFlag = true;
}

void initRadio() {
  int state = radio.begin(868.0);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("Radio OK");
    pinMode(LORA_LED, OUTPUT);
    radio.setDio1Action(setFlag);
    radio.startReceive();
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
  delay(DEVICE_ID * 1000);
  // if (!channelFree()) {delay(random(5, 25)); continue;}
  int state = radio.transmit((uint8_t*)pkt, sizeof(Packet));
  if (state == RADIOLIB_ERR_NONE) {
    packetCounter++; blink(); radio.startReceive(); return true;
  }
  // }
  radio.startReceive(); return false;
}

bool receivePacket(Packet* pkt) {
  if (!receivedFlag) return false;
  receivedFlag = false;
  int state = radio.readData((uint8_t*)pkt, sizeof(Packet));
  timeOfArrival = millis();
  radio.startReceive();
  if (state != RADIOLIB_ERR_NONE) return false;
  if (pkt->sender == DEVICE_ID) return false;
  if (pkt->receiver != DEVICE_ID && pkt->receiver != 255) return false;
  blink(); return true;
}