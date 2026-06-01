#include <Arduino.h>
#include <RadioLib.h>
#include "Globals.h"
#include "Packet.h"
#include "RadioManager.h"

void powerConsumption();

bool diagnoseDevice(uint8_t SID, uint8_t RID);

bool syncClocks(uint8_t SID);

bool pingDevices(uint8_t SID);

void relayPing(uint8_t SID, uint8_t RRID, uint8_t RID, bool flag);

void networkDiagnostics();