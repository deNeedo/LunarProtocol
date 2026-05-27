#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>

struct __attribute__((packed)) Packet {
  uint8_t type; // command type, 1 = PING; 2 = ACK; 3 = SYNC; 4 = RELAY
  uint8_t sender; //  should always be set to DEVICE_ID
  uint8_t receiver; // 0 = HUB; N = ROVER nr N; 255 = ALL
  uint16_t packetID; // autoincrementing packet number for detection of chunk transmission
  uint8_t hopCount; // this value is for determining if packet can be retransmitted, upon retransmission it should be reduced by 1 when compared with received packet
  uint32_t processing; // this value is used for replying to tell how much time is took to process response, 0 for init packets
  char payload[10];
};

inline Packet createPacket(uint8_t type, uint8_t sender, uint8_t receiver, uint16_t packetID, uint8_t hopCount, uint32_t processing, const char* payload) {
  Packet pkt;
  pkt.type = type;
  pkt.sender = sender;
  pkt.receiver = receiver;
  pkt.packetID = packetID;
  pkt.hopCount = hopCount;
  pkt.processing = processing;
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
  Serial.print("Processing: ");
  Serial.println(pkt.processing);
  Serial.print("Payload: ");
  for (size_t i = 0; i < sizeof(pkt.payload); i++) {
    if (pkt.payload[i] == '\0') break;
    Serial.print(pkt.payload[i]);
  }
  Serial.println();
  Serial.println("------------------");
}

#endif