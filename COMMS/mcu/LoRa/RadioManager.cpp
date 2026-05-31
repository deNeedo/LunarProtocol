// Libraries
#include "Globals.h"
#include "RadioManager.h"

SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY);

volatile bool receivedFlag = false;

void setFlag() {
  receivedFlag = true;
}

void initRadio() {
  int state = radio.begin(868.6, 125.0, 12, 8);
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

bool checkReadiness() {
  for (int m = 0; m < TOTAL_DEVICES; m++) {
    if (DEVICE_ID == m) continue;
    Serial.printf("DEVICE_%d: DEVICE_%d status: %d\n", DEVICE_ID, m, (int)ackArray[m]);
    if (!ackArray[m]) return false;
  }
  memset(ackArray, false, sizeof ackArray);
  return true;
}

bool sendPacket(Packet* pkt) {
  int state = radio.transmit((uint8_t*)pkt, sizeof(Packet));
  if (state != RADIOLIB_ERR_NONE) return false;
  packetCounter++; return true;
}

bool receivePacket(Packet* pkt, uint8_t expectedType) {
  int state = radio.receive((uint8_t*)pkt, sizeof(Packet));
  if (state != RADIOLIB_ERR_NONE) return false;
  if (pkt->type != expectedType) return false;
  if (pkt->sender == DEVICE_ID) return false;
  if (pkt->receiver != DEVICE_ID && pkt->receiver != 255) return false;
  blink(); return true;
}

uint32_t timeNow() {
  return (millis() - syncTimeLocal);
}
