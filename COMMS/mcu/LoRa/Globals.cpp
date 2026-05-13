#include "Globals.h"

uint8_t DEVICE_ID = 2;
uint8_t TOTAL_DEVICES = 2;

uint16_t packetCounter = 0;
bool waitingForAck = false;

uint32_t LISTEN_TIME = 10000;
uint32_t lastPing = 0;
uint32_t beginPing = 0;
uint32_t endPing = 0;

uint32_t timeOfArrival = 0;
uint32_t timeSpent = 0;

enum State {IDLE, WAITING_FOR_ACK};