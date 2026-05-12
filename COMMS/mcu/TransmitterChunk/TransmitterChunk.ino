#include <RadioLib.h>

SX1262 radio = new Module(41, 39, 42, 40);

struct Packet {
  uint8_t senderId;
  uint32_t timestamp;
  float value;
};

#define MAX_PACKETS 50
#define LISTEN_TIME 5000

Packet receivedPackets[MAX_PACKETS];

int packetCount = 0;

void setup() {

  Serial.begin(115200);

  int state = radio.begin(875, 125, 7, 7);

  if(state != RADIOLIB_ERR_NONE) {
    Serial.println("Radio init failed!");
    while(true);
  }

  Serial.println("Radio ready");
}

void loop() {

  // =====================================================
  // 1. SEND OWN PACKET
  // =====================================================

  Packet myPacket;

  myPacket.senderId = 1;
  myPacket.timestamp = millis();
  myPacket.value = random(100, 300) / 10.0;

  int state = radio.transmit((uint8_t*)&myPacket,
                             sizeof(myPacket));

  if(state == RADIOLIB_ERR_NONE) {
    Serial.println("Packet sent");
  } else {
    Serial.print("Send failed: ");
    Serial.println(state);
  }

  // =====================================================
  // 2. START LISTENING WINDOW
  // =====================================================

  packetCount = 0;

  uint32_t listenStart = millis();

  Serial.println("Listening...");

  while(millis() - listenStart < LISTEN_TIME) {

    Packet incomingPacket;

    state = radio.receive((uint8_t*)&incomingPacket,
                          sizeof(incomingPacket));

    if(state == RADIOLIB_ERR_NONE) {

      // ignore own packets
      if(incomingPacket.senderId == myPacket.senderId) {
        continue;
      }

      if(packetCount < MAX_PACKETS) {

        receivedPackets[packetCount] = incomingPacket;

        Serial.print("Received from ID ");
        Serial.print(incomingPacket.senderId);

        Serial.print(" value=");
        Serial.println(incomingPacket.value);

        packetCount++;
      }
    }
  }

  // =====================================================
  // 3. PROCESS RECEIVED PACKETS
  // =====================================================

  Serial.println();
  Serial.println("=== PROCESSING ===");

  sortPackets();

  for(int i = 0; i < packetCount; i++) {

    Serial.print("ID: ");
    Serial.print(receivedPackets[i].senderId);

    Serial.print(" Time: ");
    Serial.print(receivedPackets[i].timestamp);

    Serial.print(" Value: ");
    Serial.println(receivedPackets[i].value);
  }

  Serial.println("=== END ===");
  Serial.println();

  delay(2000);
}

void sortPackets() {

  for(int i = 0; i < packetCount - 1; i++) {

    for(int j = i + 1; j < packetCount; j++) {

      if(receivedPackets[i].senderId >
         receivedPackets[j].senderId) {

        Packet temp = receivedPackets[i];

        receivedPackets[i] = receivedPackets[j];

        receivedPackets[j] = temp;
      }
    }
  }
}