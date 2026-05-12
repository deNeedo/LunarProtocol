#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>

enum RadioState {IDLE, WAITING_FOR_ACK};

extern RadioState radioState;
extern uint32_t ackTime;

void setRadioState(RadioState newState);
void updateRadioState();

#endif