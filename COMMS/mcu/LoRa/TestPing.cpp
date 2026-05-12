#include "TestPing.h"

void pingRadio() {
  // SEND PING
  uint32_t now = millis();
  if ((now - lastPing) > 10000 && radioState == IDLE && DEVICE_ID == 0) {
    Packet pkt_init = {}; pkt_init.type = 1; pkt_init.sender = DEVICE_ID; pkt_init.receiver = 255; pkt_init.packetID = packetCounter; pkt_init.hopCount = 0; strcpy(pkt_init.payload, "HELLO!");
    pkt_init.timestamp = millis();
    sendPacket(&pkt_init);
    beginPing = lastPing = millis();
    Serial.println("SENT:");
    printPacket(pkt_init);
    setRadioState(WAITING_FOR_ACK);
  }
  if (radioState == WAITING_FOR_ACK && DEVICE_ID == 0) {
    Packet receivedPackets[5]; uint8_t packetCount = 0;
    while(millis() - beginPing < 5000) {
      Packet pkt_ack = {};
      // receivePacket(&pkt_ack);
      int state = radio.receive((uint8_t*)&pkt_ack, sizeof(pkt_ack));
      if (state == RADIOLIB_ERR_NONE && packetCount < 5) {
        printPacket(pkt_ack);
        receivedPackets[packetCount] = pkt_ack;
        packetCount++;
      }
      Serial.print("DUPA");
    }
  }
  if (radioState == IDLE && DEVICE_ID != 0) {
    Packet pkt_in = {}; Packet pkt_out = {};
    if (receivePacket(&pkt_in)) {
      Serial.println("RECEIVED:");
      printPacket(pkt_in);
      // RECEIVED PING
      if (pkt_in.type == 1) {
        pkt_out.type = 2; pkt_out.sender = DEVICE_ID; pkt_out.receiver = pkt_in.sender; pkt_out.packetID = packetCounter; pkt_out.hopCount = 0;
        uint32_t timeOfDeparture = millis(); timeSpent = timeOfDeparture - timeOfArrival;
        memcpy(pkt_out.payload, &timeSpent, sizeof(timeSpent)); pkt_out.timestamp = timeOfDeparture;
        sendPacket(&pkt_out);
        Serial.println("SENT:");
        printPacket(pkt_out);
      }
      // RECEIVED ACK
      else if (pkt_in.type == 2) {
        endPing = millis(); uint32_t totalRTT = endPing - beginPing;
        Serial.print("RTT: "); Serial.println(totalRTT);
        uint32_t remoteDelay; memcpy(&remoteDelay, pkt_in.payload, sizeof(remoteDelay));
        Serial.print("Remote delay: "); Serial.println(remoteDelay);
        float propTime = (totalRTT - remoteDelay) / 2.0;
        Serial.print("Estimated propagation: "); Serial.println(propTime);
        setRadioState(IDLE);
      }
    }
  }



  // RECEIVE
  Packet pkt_out = {}; Packet pkt_in = {};

  updateRadioState();
}