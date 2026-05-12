#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>

struct __attribute__((packed)) Packet {
  uint8_t type; // 1 = PING; 2 = ACK; 3 = ...
  uint8_t sender;
  uint8_t receiver; // 0 = HUB; N = ROVER nr N; 255 = ALL
  uint16_t packetID;
  uint8_t hopCount;
  uint32_t timestamp;
  char payload[10];
};

inline void printPacket(const Packet& pkt) {
  Serial.println("----- PACKET -----");
  Serial.print("Type: ");
  Serial.println(pkt.type);
  Serial.print("Sender: ");
  Serial.println(pkt.sender);
  Serial.print("Receiver: ");
  Serial.println(pkt.receiver);
  Serial.print("PacketID: ");
  Serial.println(pkt.packetID);
  Serial.print("HopCount: ");
  Serial.println(pkt.hopCount);
  Serial.print("Timestamp: ");
  Serial.println(pkt.timestamp);
  Serial.println("------------------");
}

#endif