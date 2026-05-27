#include "Globals.h"

bool ackArray[TOTAL_DEVICES] = {};

uint8_t messageCounter = 0;
uint16_t packetCounter = 0;
uint16_t pingResult = 0;
uint32_t syncedTime = 0;
uint32_t syncedTimeRelay = 0;
uint32_t pingTimings[MESSAGE_CHUNK_SIZE] = {};

Packet empty = {};
Packet pktIn = {};
Packet pktOut = {};
// Packet incomingQueue[(TOTAL_DEVICES - 1) * MESSAGE_CHUNK_SIZE] = {};
// Packet outgoingQueue[(TOTAL_DEVICES - 1) * MESSAGE_CHUNK_SIZE] = {};