#include "TestSuite.h"

bool diagnoseDevice(uint8_t SID, uint8_t RID) {
  if (SID == RID) {pingResult = 0; return false;}
  if (DEVICE_ID == SID) {
    messageCounter = 0;
    pktOut = createPacket(1, DEVICE_ID, RID, packetCounter, 0, millis(), "PING");
    sendPacket(&pktOut);
    uint32_t beginPing = pktOut.processing;
    while(millis() < beginPing + PING_TIMEOUT) {
      if (receivePacket(&pktIn, 2)) {
        uint32_t endPing = millis();
        uint32_t ping = (endPing - (beginPing + pktIn.processing)) / 2;
        float rssi = radio.getRSSI(true); float snr  = radio.getSNR();
        Serial.printf("DEVICE_%d: RSSI: %f | SNR: %f | PING: %lu ms.\n", DEVICE_ID, rssi, snr, ping);
        return true;
      }
    }
    Serial.printf("DEVICE_%d: Unable to access DEVICE_%d.\n", DEVICE_ID, RID);
    return false;
  }
  if (DEVICE_ID == RID) {
    if (receivePacket(&pktIn, 1)) {
      pktOut = createPacket(2, DEVICE_ID, pktIn.sender, packetCounter, 0, 0, "PONG");
      sendPacket(&pktOut);
      return true;
    } else {
      return false;
    }
  }
}

bool syncClocks() {
  memset(reachable, false, sizeof(reachable));
  memset(incomingQueue, 0, sizeof(incomingQueue));
  memset(pingTimings, 0, sizeof(pingTimings));

  uint8_t ackCount = 0; bool roverSyncFailure = false; bool hubSynced = false;

  // ==================================================
  // HUB
  // ==================================================

  if (DEVICE_ID == 0) {
    pktOut = createPacket(5, DEVICE_ID, 255, packetCounter, 0, millis(), "SYNC");
    syncTimeLocal = pktOut.processing; sendPacket(&pktOut);
    Serial.printf("DEVICE_%d: Sending SYNC packet.\n", DEVICE_ID);

    uint32_t timeout = syncTimeLocal + TOTAL_DEVICES * SLOT_DELAY;

    while (millis() < timeout) {
      if (receivePacket(&pktIn, 6)) {
        uint8_t sender = pktIn.sender;
        if (!reachable[sender]) {
          reachable[sender] = true;
          incomingQueue[sender] = pktIn;
          pingTimings[sender] = millis();
          ackCount++;
          Serial.printf("DEVICE_%d: Received ACK_SYNC from DEVICE_%d.\n", DEVICE_ID, sender);
          if (ackCount >= TOTAL_DEVICES - 1) {break;}
        }
      }
    }

    for (uint8_t d = 1; d < TOTAL_DEVICES; d++) {
      if (!reachable[d]) {
        Serial.printf("DEVICE_%d: Cannot reach DEVICE_%d.\n", DEVICE_ID, d); roverSyncFailure = true; continue;
      }
      if (!hubSynced) {
        uint32_t obtaining = pingTimings[d];
        uint32_t processing = incomingQueue[d].processing;
        uint32_t timeCorrection = (obtaining - (syncTimeLocal + processing)) / 2;
        syncTimeLocal += timeCorrection; hubSynced = true;
      }
    }
    
    if (roverSyncFailure) return false;
    pktOut = createPacket(2, DEVICE_ID, 255, packetCounter, 0, 0, "ACK_SYNC");
    sendPacket(&pktOut);
    return true;
  }

  // ==================================================
  // ROVERS
  // ==================================================

  while (true) {
    if (receivePacket(&pktIn, 5)) {
      syncTimeLocal = millis();
      Serial.printf("DEVICE_%d: Received SYNC packet.\n", DEVICE_ID); break;
    }
  }

  uint32_t slotTime = syncTimeLocal + FLAT_DELAY + DEVICE_ID * SLOT_DELAY;

  while (millis() < slotTime) {/*WAIT*/}
  pktOut = createPacket(6, DEVICE_ID, 0, packetCounter, 0, 0, "ACK_SYNC");
  pktOut.processing = millis() - syncTimeLocal;
  sendPacket(&pktOut);
  Serial.printf("DEVICE_%d: Sending ACK_SYNC packet.\n", DEVICE_ID);
  return true;
}


bool pingDevices(uint8_t SID) {

  // ==========================================
  // HUB
  // ==========================================

  if (DEVICE_ID == SID) {
    outOfRange = 0;
    memset(pingTimings, 0, sizeof(pingTimings));
    memset(reachable, false, sizeof(reachable));
    pktOut = createPacket(1, DEVICE_ID, 255, packetCounter, 1, timeNow(), "PING");
    uint32_t pingStart = pktOut.processing;
    sendPacket(&pktOut);
    Serial.printf("DEVICE_%d: Sending PING packet.\n", DEVICE_ID);
    uint32_t timeout = pingStart + TOTAL_DEVICES * PING_TIMEOUT;
    uint8_t results = 0;
    while (timeNow() < timeout) {
      if (receivePacket(&pktIn, 2)) {
        uint8_t sender = pktIn.sender;
        reachable[sender] = true;
        pingTimings[sender] = pktIn.processing - pingStart;
        Serial.printf("DEVICE_%d: Received PONG packet from DEVICE_%d with ping: %lu ms.\n", DEVICE_ID, sender, pingTimings[sender]);
        results++;
      }
      if (results >= TOTAL_DEVICES - 1) return true;
    }
    for (int m = 0; m < TOTAL_DEVICES; m++) {
      if (!reachable[m]) {
        if (m == DEVICE_ID) continue;
        outOfRange += m;
        Serial.printf("DEVICE_%d: Unable to ping DEVICE_%d. Trying RELAY method...\n", DEVICE_ID, m);
      }
    }
    return false;
  }

  // ==========================================
  // ROVER
  // ==========================================

  if (receivePacket(&pktIn, 1)) {
    uint32_t receiveTime = timeNow();
    uint32_t slotTime = receiveTime + FLAT_DELAY + DEVICE_ID * PING_TIMEOUT;
    Serial.printf("DEVICE_%d: Received PING packet from DEVICE_%d with ping: %lu ms.\n", DEVICE_ID, pktIn.sender, (receiveTime - pktIn.processing));
    while (timeNow() < slotTime) {}
    pktOut = createPacket(2, DEVICE_ID, 0, packetCounter, 0, receiveTime, "ACK");
    /*SOFTWARE BLOCKADE OF COMMUNICATION*/
    // if (DEVICE_ID == 1 && commsBlock) return false;
    if (DEVICE_ID == 2 && commsBlock) return false;
    /*SOFTWARE BLOCKADE OF COMMUNICATION*/
    sendPacket(&pktOut);
    Serial.printf("DEVICE_%d: Sending PONG packet.\n", DEVICE_ID);
    return true;
  }
  return false;
}

void relayPing(uint8_t SID, uint8_t RRID, uint8_t RID, bool flag) {
  uint32_t temp; uint8_t sender; uint8_t number;
  if (DEVICE_ID == SID && !flag) {
    while (timeNow() % CYCLE < (CYCLE_RELAY + CYCLE_OFFSET)) {/*WAIT*/}
    if (outOfRange == RRID) {number = RRID; RRID = RID; RID = number;}
    else if (outOfRange == (RRID + RID)) {return;}
    Serial.printf("DEVICE_%d: Sending RELAY packet to DEVICE_%d through DEVICE_%d.\n", DEVICE_ID, RID, RRID);
    temp = timeNow();
    pktOut = createPacket(3, DEVICE_ID, RRID, packetCounter, 0, temp, "RELAY");
    sendPacket(&pktOut);
    while ((timeNow() % CYCLE >= (CYCLE_RELAY + CYCLE_OFFSET)) && (timeNow() % CYCLE < (CYCLE - 3 * CYCLE_OFFSET))) {
      if (receivePacket(&pktIn, 4)) {
        sender = pktIn.sender;
        Serial.printf("DEVICE_%d: Received ACK_RELAY packet from DEVICE_%d through DEVICE_%d with ping: %lu ms.\n", DEVICE_ID, sender, RRID, (pktIn.processing - temp));
        break;
      }
    }
  }
  else if (DEVICE_ID == RRID && !flag) {number = RRID; RRID = RID; RID = number;}
  else if (DEVICE_ID == RID && flag) {number = RRID; RRID = RID; RID = number;}
  if (DEVICE_ID == RRID) {
    while (timeNow() % CYCLE < CYCLE_RELAY) {/*WAIT*/}
    // Serial.printf("Hello from relayPing, I am DEVICE %d and my flag is set to %d\n", DEVICE_ID, flag);
    while ((timeNow() % CYCLE >= CYCLE_RELAY) && (timeNow() % CYCLE < (CYCLE - 3 * CYCLE_OFFSET))) {
      if (receivePacket(&pktIn, 3)) {
        temp = timeNow();
        sender = pktIn.sender;
        Serial.printf("DEVICE_%d: Received RELAY packet from DEVICE_%d for DEVICE_%d with ping: %lu ms. Forwarding now...\n", DEVICE_ID, sender, RID, (temp - pktIn.processing));
        pktOut = pktIn;
        pktOut.receiver = RID;
        sendPacket(&pktOut); break;
      }
    }
    while ((timeNow() % CYCLE >= CYCLE_RELAY) && (timeNow() % CYCLE < (CYCLE - 3 * CYCLE_OFFSET))) {
      if (receivePacket(&pktIn, 4)) {
        temp = timeNow();
        sender = pktIn.sender;
        Serial.printf("DEVICE_%d: Received ACK_RELAY packet from DEVICE_%d for DEVICE_%d with ping: %lu ms. Forwarding now...\n", DEVICE_ID, sender, SID, (temp - pktIn.processing));
        pktOut = pktIn;
        pktOut.receiver = SID;
        sendPacket(&pktOut); break;
      }
    }
  }
  
  if (DEVICE_ID == RID) {
    while (timeNow() % CYCLE < (CYCLE_RELAY - CYCLE_OFFSET)) {/*WAIT*/}
    // Serial.printf("Hello from relayPing, I am DEVICE %d and my flag is set to %d\n", DEVICE_ID, flag);
    while ((timeNow() % CYCLE >= (CYCLE_RELAY - CYCLE_OFFSET)) && (timeNow() % CYCLE < (CYCLE - 3 * CYCLE_OFFSET))) {
      if (receivePacket(&pktIn, 3)) {
        temp = timeNow();
        sender = pktIn.sender;
        Serial.printf("DEVICE_%d: Received RELAY packet from DEVICE_%d through DEVICE_%d with ping: %lu ms.\n", DEVICE_ID, sender, RRID, (temp - pktIn.processing));
        pktOut = createPacket(4, DEVICE_ID, RRID, packetCounter, 0, temp, "ACK_RELAY");
        Serial.printf("DEVICE_%d: Sending ACK_RELAY packet to DEVICE_%d through DEVICE_%d.\n", DEVICE_ID, SID, RRID);
        sendPacket(&pktOut); break;
      }
    }
  }
}

void networkDiagnostics() {
  if (readyToPlay == false) {
    if (syncClocks()) {readyToPlay = true;}
    if (DEVICE_ID != 0) {
      uint32_t timeout = syncTimeLocal + TOTAL_DEVICES * SLOT_DELAY;
      while (millis() < timeout) {
        readyToPlay = false;
        if (receivePacket(&pktIn, 2)) {
          readyToPlay = true; break;
        }
      }
    }
  } else {
    Serial.printf("DEVICE_%d: Sync succesful. Current time: %lu ms.", DEVICE_ID, timeNow());
    while (true) {
      if (timeNow() >= 50000 && !commsBlock) {commsBlock = true;} // software block for comms
      if (timeNow() % CYCLE < CYCLE_DELAY) {
        bool flag = pingDevices(0);
        relayPing(0, 1, 2, flag);
      }
    }
  }
}

