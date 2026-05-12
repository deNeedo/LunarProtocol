#include "StateMachine.h"

RadioState radioState = IDLE;
uint32_t ackTime = 0;

void setRadioState(RadioState newState) {
  radioState = newState;
  if (newState == WAITING_FOR_ACK) {ackTime = millis();}
}

void updateRadioState() {
  if (radioState == WAITING_FOR_ACK) {
    if (millis() - ackTime > 10000) {
      Serial.println("ACK timeout"); radioState = IDLE;
    }
  }
}