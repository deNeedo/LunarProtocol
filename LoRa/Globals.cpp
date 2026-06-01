#include "Globals.h"

bool ackArray[TOTAL_DEVICES] = {};
bool readyToPlay = false;
bool powerOn = true;

uint8_t outOfRange = 0;
bool commsBlock = false;

uint8_t messageCounter = 0;
uint16_t packetCounter = 0;
uint16_t pingResult = 0;
uint32_t syncedTime = 0;
uint32_t syncedTimeRelay = 0;
// uint32_t pingTimings[MESSAGE_CHUNK_SIZE] = {};

uint8_t ackCount = 0;
uint32_t syncSentAt = 0;
uint32_t syncRxAt = 0;
Packet ackBuffer[TOTAL_DEVICES] = {};

Packet incomingQueue[TOTAL_DEVICES];
uint32_t pingTimings[TOTAL_DEVICES];

bool reachable[TOTAL_DEVICES];

uint32_t syncTimeLocal = 0;
uint32_t syncTimeHub = 0;

Packet empty = {};
Packet pktIn = {};
Packet pktOut = {};
// Packet incomingQueue[TOTAL_DEVICES * MESSAGE_CHUNK_SIZE] = {};
// Packet outgoingQueue[TOTAL_DEVICES * MESSAGE_CHUNK_SIZE] = {};