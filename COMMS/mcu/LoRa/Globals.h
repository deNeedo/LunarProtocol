#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

extern uint8_t DEVICE_ID;

extern uint16_t packetCounter;
extern bool waitingForAck;

extern uint32_t lastPing;
extern uint32_t beginPing;
extern uint32_t endPing;

extern uint32_t timeOfArrival;
extern uint32_t timeSpent;

#endif