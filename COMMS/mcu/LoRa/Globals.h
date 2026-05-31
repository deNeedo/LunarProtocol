#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include "Packet.h"

#define DEVICE_ID 1
#define TOTAL_DEVICES 3
#define SLOT_DELAY 4000
#define FLAT_DELAY 500
#define MESSAGE_CHUNK_SIZE 3
#define PING_TIMEOUT 3000
#define CYCLE 30000
#define CYCLE_DELAY 3000
#define CYCLE_RELAY 20000
#define CYCLE_OFFSET 1000

extern bool ackArray[TOTAL_DEVICES];
extern bool readyToPlay;

extern uint8_t outOfRange;
extern bool commsBlock;

extern uint8_t messageCounter;
extern uint16_t packetCounter;
extern uint16_t pingResult;
extern uint32_t syncedTime;
extern uint32_t syncedTimeRelay;
extern uint32_t pingTimings[TOTAL_DEVICES];

extern uint8_t ackCount;
extern uint32_t syncSentAt;
extern uint32_t syncRxAt;
extern Packet ackBuffer[TOTAL_DEVICES];

extern Packet incomingQueue[TOTAL_DEVICES];

extern bool reachable[TOTAL_DEVICES];

extern uint32_t syncTimeLocal;
extern uint32_t syncTimeHub;

extern Packet empty;
extern Packet pktIn;
extern Packet pktOut;
// extern Packet incomingQueue[TOTAL_DEVICES * MESSAGE_CHUNK_SIZE];
// extern Packet outgoingQueue[TOTAL_DEVICES * MESSAGE_CHUNK_SIZE];

#endif