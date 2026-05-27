#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include "Packet.h"

#define DEVICE_ID 1
#define TOTAL_DEVICES 3
#define PING_TIMEOUT 3000
#define MESSAGE_CHUNK_SIZE 3

extern bool ackArray[TOTAL_DEVICES];

extern uint8_t messageCounter;
extern uint16_t packetCounter;
extern uint16_t pingResult;
extern uint32_t syncedTime;
extern uint32_t syncedTimeRelay;
extern uint32_t pingTimings[MESSAGE_CHUNK_SIZE];

extern Packet empty;
extern Packet pktIn;
extern Packet pktOut;
// extern Packet incomingQueue[(TOTAL_DEVICES - 1) * MESSAGE_CHUNK_SIZE];
// extern Packet outgoingQueue[(TOTAL_DEVICES - 1) * MESSAGE_CHUNK_SIZE];

#endif