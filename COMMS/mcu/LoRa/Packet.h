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

inline Packet createPacket(uint8_t type, uint8_t sender, uint8_t receiver, uint16_t packetID, uint8_t hopCount, uint32_t timestamp, const char* payload) {
  Packet pkt;
  pkt.type = type;
  pkt.sender = sender;
  pkt.receiver = receiver;
  pkt.packetID = packetID;
  pkt.hopCount = hopCount;
  pkt.timestamp = timestamp;
  strncpy(pkt.payload, payload, sizeof(pkt.payload));
  pkt.payload[sizeof(pkt.payload) - 1] = '\0';
  return pkt;
}

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
  Serial.print("Payload: ");
  for (size_t i = 0; i < sizeof(pkt.payload); i++) {
    if (pkt.payload[i] == '\0') break;
    Serial.print(pkt.payload[i]);
  }
  Serial.println();
  Serial.println("------------------");
}

#endif