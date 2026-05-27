#include "TestSuite.h"

bool pingDevice(uint8_t SID, uint8_t RID) {
  if (SID == RID) {pingResult = 0; return false;}
  if (DEVICE_ID == SID) {
    messageCounter = 0; memset(pingTimings, 0, sizeof pingTimings);
    pktOut = createPacket(1, DEVICE_ID, RID, packetCounter, 0, millis(), "PING");
    sendPacket(&pktOut, 1);
    uint32_t beginPing = pktOut.processing;
    while(millis() - beginPing < PING_TIMEOUT) {
      if (receivePacket(&pktIn, 2)) {
        uint32_t endPing = millis();
        pingTimings[messageCounter] = (endPing - (beginPing + pktIn.processing)) / 2;
        messageCounter++;
      }
    }
    uint16_t avgPing = 0; uint8_t avgSamples = 0;
    for (int m = 0; m < MESSAGE_CHUNK_SIZE; m++) {
      if (pingTimings[m] == 0) continue;
      avgPing += pingTimings[m]; avgSamples++;
    }
    if (avgSamples == 0) {
      pingResult = 0;
      Serial.printf("Unable to ping rover %d\n", RID);
      return true;
    } else {
      pingResult = avgPing / avgSamples;
      Serial.printf("Average ping to rover %d: %lu\n", RID, pingResult);
      return true;
    }
  }
  if (DEVICE_ID == RID) {
    if (receivePacket(&pktIn, 1)) {
      /*ADDITIONAL TEST*/
      if (pktIn.sender == 0 && pktIn.receiver == 1) {return false;}
      /*ADDITIONAL TEST*/
      uint32_t timeOfArrival = millis();
      for (int m = 0; m < MESSAGE_CHUNK_SIZE; m++) {
        uint32_t timeOfDeparture = millis();
        pktOut = createPacket(2, DEVICE_ID, pktIn.sender, packetCounter, 0, (timeOfDeparture - timeOfArrival), "PONG");
        sendPacket(&pktOut, 1);
      }
      return true;
    } else {
      return false;
    }
  }
}

void measureDistance(uint8_t SID, uint8_t RID) {
  if (DEVICE_ID == SID) {
    pktOut = createPacket(1, DEVICE_ID, RID, packetCounter, 0, millis(), "PING");
    sendPacket(&pktOut, 1);
    Serial.printf("DEVICE_%d: Packet sent.\n", DEVICE_ID);
    if (receivePacket(&pktIn, 2)) {
      float rssi = radio.getRSSI(true); float snr  = radio.getSNR();
      Serial.printf("DEVICE_%d: Packet received. RSSI: %f | SNR: %f\n", DEVICE_ID, rssi, snr);
    }
  }
  if (DEVICE_ID == RID) {
    if (receivePacket(&pktIn, 1)) {
      float rssi = radio.getRSSI(true); float snr  = radio.getSNR();
      Serial.printf("DEVICE_%d: Packet received. RSSI: %f | SNR: %f\n", DEVICE_ID, rssi, snr);
      pktOut = createPacket(2, DEVICE_ID, pktIn.sender, packetCounter, 0, millis(), "PONG");
      sendPacket(&pktOut, 1);
      Serial.printf("DEVICE_%d: Packet sent.\n", DEVICE_ID);
    }
  }
}

bool clockSync(uint8_t SID, bool syncedFlag) {
  /*START SYNC PROCESS*/
  if (DEVICE_ID != SID) {
    while (true) {
      Serial.printf("DEVICE_%d: Waiting for SYNC packet.\n", DEVICE_ID);
      if (receivePacket(&pktIn, 5)) {
        Serial.printf("DEVICE_%d: Received SYNC packet from DEVICE_%d.\n", DEVICE_ID, pktIn.sender);
        if (syncedFlag) {
          syncedTimeRelay = millis();
        } else {
          syncedTime = millis();
        } break;
      }
    }
  } else if (DEVICE_ID == SID) {
    pktOut = createPacket(5, DEVICE_ID, 255, packetCounter, 0, 0, "SYNC");
    sendPacket(&pktOut, MESSAGE_CHUNK_SIZE); packetCounter++;
    Serial.printf("DEVICE_%d: Sent SYNC packets.\n", DEVICE_ID);
    if (syncedFlag) {
      syncedTimeRelay = millis();
    } else {
      syncedTime = millis();
    }
  }
  /*RESET CONTROL VARIABLES*/
  memset(ackArray, false, sizeof ackArray);
  /*LISTEN IN DESIGNATED TIME WINDOWS*/
  uint32_t compTime;
  if (syncedFlag) {
    compTime = syncedTimeRelay;
  } else {
    compTime = syncedTime;
  }
  while ((millis() - compTime) < (DEVICE_ID * PING_TIMEOUT)) {
    if (receivePacket(&pktIn, 6)) {
      Serial.printf("DEVICE_%d: Received ACK_SYNC packet from DEVICE_%d.\n", DEVICE_ID, pktIn.sender);
      ackArray[pktIn.sender] = true; pktIn = empty;
      /*incomingQueue[messageCounter] = pktIn; messageCounter++;*/ 
    }
  }
  pktOut = createPacket(6, DEVICE_ID, 255, packetCounter, 0, 0, "ACK_SYNC");
  sendPacket(&pktOut, MESSAGE_CHUNK_SIZE); packetCounter++;
  Serial.printf("DEVICE_%d: Sent ACN_SYNC packets.", DEVICE_ID);
  while ((millis() - compTime) < (TOTAL_DEVICES * PING_TIMEOUT)) {
    if (receivePacket(&pktIn, 6)) {
      Serial.printf("DEVICE_%d: Received ACK_SYNC packet from DEVICE_%d.\n", DEVICE_ID, pktIn.sender);
      ackArray[pktIn.sender] = true; pktIn = empty;
      /*incomingQueue[messageCounter] = pktIn; messageCounter++;*/
    }
  }
  Serial.printf("DEVICE_%d: Sync process completed.\n", DEVICE_ID);
  for (int m = 0; m < TOTAL_DEVICES; m++) {
    if (DEVICE_ID == m) continue;
    if (ackArray[m] == 0) return false;
    Serial.printf("DEVICE_%d: Device synced with DEVICE_%d? %d\n", DEVICE_ID, m, ackArray[m]);
  }
  return true;
}

void startSync(uint8_t SID) {
  if (DEVICE_ID != SID) {
    while (true) {
      Serial.printf("DEVICE_%d: Waiting for SYNC packet.\n", DEVICE_ID);
      if (receivePacket(&pktIn, 3)) {syncedTime = millis(); break;}
    }
  } else if (DEVICE_ID == SID) {
    pktOut = createPacket(3, DEVICE_ID, 255, packetCounter, 0, 0, "SYNC");
    sendPacket(&pktOut, MESSAGE_CHUNK_SIZE); packetCounter++;
    Serial.printf("DEVICE_%d: Sent SYNC packets.\n", DEVICE_ID);
    syncedTime = millis();
  }
}

void syncClocks(uint8_t SID) {
  memset(ackArray, false, sizeof ackArray); 
  if (DEVICE_ID != SID) {
    while (true) {
      if (pingDevice(SID, DEVICE_ID)) {
        break;
      }
    }
    // while (true) {
    //   // if ()
    // }
  } else if (DEVICE_ID == SID) {
    uint8_t m = 0;
    while (m < TOTAL_DEVICES) {
      if (m == DEVICE_ID) {m++; continue;}
      if (pingDevice(DEVICE_ID, m)) {
        m++;
      }
    }
  }
}


void relayCommsTest() {
  uint8_t tries = 0;
  if (DEVICE_ID != 2) {
    while (tries < 3) {
      pingDevice(0, 1);
      if (pingResult == 0) tries++;
      else {Serial.println("Ping success"); return;}
    }
    Serial.println("Trying relay method");
    if (DEVICE_ID == 0) {
      pingDevice(0, 2);
      if (pingResult != 0) {
        delay(1000);
      }
    }
    if (DEVICE_ID == 1) {
      while (true) {
        Serial.println("Trying relay method");
        pingDevice(2, 1);
        if (pingResult != 0) break;
        delay(1000);
      }
    }
  }
  if (DEVICE_ID == 2) {
    while (true) {
      Serial.println("Trying relay method 1/2");
      pingDevice(0, 2);
      if (pingResult != 0) {
        Serial.println("Trying relay method 2/2");
        pingDevice(2, 1);
        if (pingResult != 0) {
          Serial.println("Ping success"); return;
        }
      }
    }
  }
}

void runDiagnostics() {
  // if (clockSync(0, false)) {
  //   Serial.printf("DEVICE_%d: Current time: %lu\n", DEVICE_ID, millis() - syncedTime);
  //   if (clockSync(1, true)) {
  //     Serial.printf("DEVICE_%d: Current time: %lu\n", DEVICE_ID, millis() - syncedTimeRelay);
  //     if (DEVICE_ID == 0) {
  //       if (receivePacket(&pktIn, 4)) {
  //         syncedTime = syncedTimeRelay - pktIn.processing;
  //       }
  //     } else if (DEVICE_ID == 2) {
  //       pktOut = createPacket(4, DEVICE_ID, 0, packetCounter, 0, (syncedTimeRelay - syncedTime), "CORRECT");
  //       sendPacket(&pktOut, MESSAGE_CHUNK_SIZE); packetCounter++;
  //     }
  //   }
  // }
  startSync(0);
  syncClocks(0);
}

