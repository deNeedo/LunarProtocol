#include <Arduino.h>
#include <RadioLib.h>
#include "Globals.h"
#include "Packet.h"
#include "RadioManager.h"
#include "StateMachine.h"

bool pingDevice(uint8_t SID, uint8_t RID);

void measureDistance(uint8_t SID, uint8_t RID);

bool clockSync(uint8_t SID, bool syncedFlag);

void syncClocks(uint8_t SID);

void runDiagnostics();

void relayCommsTest();