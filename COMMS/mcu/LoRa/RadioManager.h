#ifndef RADIO_H
#define RADIO_H
// Libraries
#include <RadioLib.h>
#include "Blink.h"
#include "Packet.h"
// Lora PINOUT
#define LORA_CS   41
#define LORA_DIO1 39
#define LORA_RST  42
#define LORA_BUSY 40

extern SX1262 radio;
extern volatile bool receivedFlag;

void initRadio();
bool channelFree();
bool checkReadiness();
bool sendPacket(Packet* pkt, uint8_t number);
bool receivePacket(Packet* pkt, uint8_t type);

#endif