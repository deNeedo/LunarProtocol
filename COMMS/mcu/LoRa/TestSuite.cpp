#include "TestSuite.h"

void pingDevice() {
  if (DEVICE_ID == 0) {
    for (int m = 1; m <= TOTAL_DEVICES; m++) {
      Packet arr[10]; uint8_t messageCounter = 0;
      Packet pkt = createPacket(1, DEVICE_ID, m, packetCounter, 0, millis(), "PING");
      sendPacket(&pkt);
      beginPing = pkt.timestamp;
      while(millis() - beginPing < LISTEN_TIME) {
        if (receivePacket(&pkt)) {
          if (pkt.type == 2) {
            arr[messageCounter] = pkt;
            printPacket(pkt);
            messageCounter++;
          }
        }
      }
    }
  }
  // if (radioState == WAITING_FOR_ACK && DEVICE_ID == 0) {
  //   Packet receivedPackets[5]; uint8_t packetCount = 0;
  //   while(millis() - beginPing < 5000) {
  //     Packet pkt_ack = {};
  //     // receivePacket(&pkt_ack);
  //     int state = radio.receive((uint8_t*)&pkt_ack, sizeof(pkt_ack));
  //     if (state == RADIOLIB_ERR_NONE && packetCount < 5) {
  //       printPacket(pkt_ack);
  //       receivedPackets[packetCount] = pkt_ack;
  //       packetCount++;
  //     }
  //     Serial.print("DUPA");
  //   }
  // }
  if (DEVICE_ID != 0) {
    Packet pkt_in = {}; Packet pkt_out = {};
    if (receivePacket(&pkt_in)) {
      if (pkt_in.type == 1) {
        Serial.println("Jest");
        for (int m = 0; m < 3; m++) {
          pkt_out = createPacket(2, DEVICE_ID, pkt_in.sender, packetCounter, 0, millis(), "PING");
          uint32_t timeOfDeparture = pkt_out.timestamp; timeSpent = timeOfDeparture - timeOfArrival;
          memcpy(pkt_out.payload, &timeSpent, sizeof(timeSpent));
          sendPacket(&pkt_out);
          Serial.print("Poszło: "); Serial.println(m);
        }

      }
      // // RECEIVED ACK
      // else if (pkt_in.type == 2) {
      //   endPing = millis(); uint32_t totalRTT = endPing - beginPing;
      //   Serial.print("RTT: "); Serial.println(totalRTT);
      //   uint32_t remoteDelay; memcpy(&remoteDelay, pkt_in.payload, sizeof(remoteDelay));
      //   Serial.print("Remote delay: "); Serial.println(remoteDelay);
      //   float propTime = (totalRTT - remoteDelay) / 2.0;
      //   Serial.print("Estimated propagation: "); Serial.println(propTime);
      //   setRadioState(IDLE);
      // }
    }
  }
  // updateRadioState();
}

void measureDistance() {
  Packet pkt = {};
  if (DEVICE_ID == 0) {
    pkt = createPacket(1, DEVICE_ID, 1, packetCounter, 0, millis(), "PING");
    sendPacket(&pkt);
    Serial.println("Packet sent");
    delay(1000);
  } else if (DEVICE_ID == 1) {
    if (receivePacket(&pkt)) {
      if (pkt.type == 1) {
        float rssi = radio.getRSSI(true);
        // float snr  = getLastSNR();
        // rssiSum += rssi; // snrSum += snr; // sampleCount++;
        Serial.print("RSSI: ");
        Serial.println(rssi);
      }
    }
  }
}
 



