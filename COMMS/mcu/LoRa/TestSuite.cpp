#include "TestSuite.h"

void pingDevice() {
  Packet pkt_in = {}; Packet pkt_out = {};
  if (DEVICE_ID == 0) {
    for (int m = 0; m < TOTAL_DEVICES; m++) {
      uint32_t timings[PING_MAX_MESSAGES] = {}; uint8_t messageCounter = 0; uint32_t beginPing = millis();
      pkt_out = createPacket(1, DEVICE_ID, (m + 1), packetCounter, TOTAL_DEVICES, beginPing, "PING");
      sendPacket(&pkt_out);
      while(millis() - beginPing < PING_TIMEOUT) {
        if (receivePacket(&pkt_in)) {
          uint32_t endPing = millis();
          if (pkt_in.type == 2) {
            timings[messageCounter] = (endPing - (beginPing + pkt_in.processing));
            messageCounter++;
          }
        }
      }
      uint16_t avgPing = 0; uint8_t avgSamples = 0;
      for (int n = 0; n < PING_MAX_MESSAGES; n++) {
        if (timings[n] == 0) continue;
        avgPing += timings[n]; avgSamples++;
      }
      Serial.printf("Average ping to rover %d: %lu\n", (m + 1), (avgPing / avgSamples));
    }
  }
  if (DEVICE_ID != 0) {
    if (receivePacket(&pkt_in)) {
      uint32_t timeOfArrival = millis();
      if (pkt_in.type == 1) {
        for (int m = 0; m < 3; m++) {
          pkt_out = createPacket(2, DEVICE_ID, pkt_in.sender, packetCounter, 0, millis(), "PONG");
          uint32_t timeOfDeparture = pkt_out.processing; pkt_out.processing = (timeOfDeparture - timeOfArrival);
          sendPacket(&pkt_out);
        }
      }
    }
  }
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
        float snr  = radio.getSNR();
        // rssiSum += rssi; // snrSum += snr; // sampleCount++;
        Serial.print("RSSI: ");
        Serial.println(rssi);
      }
    }
  }
}

void clockSync() {
  uint8_t totalPackets = 6; uint32_t startTime = millis(); uint8_t arrivedCounter = 0;
  Packet pkt_in = {}; Packet pkt_out = {}; Packet pkt_arr[totalPackets] = {}; 
  if (DEVICE_ID != 0) {
    Serial.println("Waiting for start");
    while (true) {
      if (receivePacket(&pkt_in)) {
        if (pkt_in.type == 5) /*start signal*/ {
          Serial.printf("Device %d received start packet from device %d\n", DEVICE_ID, pkt_in.sender);
          startTime = millis();
          Serial.printf("Clocks syncronized: %lu\n", (millis() - startTime));
          break;
        }
      }
    }
  } else {
    pkt_out = createPacket(5, DEVICE_ID, 255, packetCounter, 0, millis(), "START");
    sendPacket(&pkt_out); packetCounter++;
    Serial.printf("Device %d send start packet\n", DEVICE_ID);
    if (DEVICE_ID == 0) {startTime = millis();}
  }
  while ((millis() - startTime) < (DEVICE_ID * PING_TIMEOUT)) {
    if (receivePacket(&pkt_in)) {
      if (pkt_in.type == 1) {
        Serial.printf("Device %d received packet from device %d\n", DEVICE_ID, pkt_in.sender);
        pkt_arr[arrivedCounter] = pkt_in; arrivedCounter++;
      }
    }
  }
  pkt_out = createPacket(1, DEVICE_ID, 255, packetCounter, 0, millis(), "PING");
  sendPacket(&pkt_out); sendPacket(&pkt_out); sendPacket(&pkt_out); packetCounter++;
  Serial.printf("Packets sent by device %d at local time: %lu\n", DEVICE_ID, (millis() - startTime));
  while ((millis() - startTime) < (TOTAL_DEVICES * PING_TIMEOUT)) {
    if (receivePacket(&pkt_in)) {
      if (pkt_in.type == 1) {
        pkt_arr[arrivedCounter] = pkt_in; arrivedCounter++;
        Serial.printf("Device %d received packet from device %d\n", DEVICE_ID, pkt_in.sender);
      }
    }
  }
  // Serial.println("Received packets:");
  // for (int m = 0; m < totalPackets; m++) {
  //   printPacket(pkt_arr[m]);
  // }
}



