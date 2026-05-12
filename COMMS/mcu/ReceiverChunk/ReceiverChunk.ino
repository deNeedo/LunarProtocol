#include <RadioLib.h>

SX1262 radio = new Module(41, 39, 42, 40);

struct Packet {

  uint8_t senderId;

  uint8_t packetNumber;
  uint8_t totalPackets;

  uint32_t timestamp;

  float value;
};

const uint8_t MY_ID = 2;

void setup() {

  Serial.begin(115200);

  int state = radio.begin(875, 125, 7, 7);

  if(state != RADIOLIB_ERR_NONE) {

    Serial.println("Radio init failed!");

    while(true);
  }

  Serial.println("Multi-packet responder ready");
}

void loop() {

  Packet incomingPacket;

  // =========================================
  // WAIT FOR REQUEST
  // =========================================

  int state =
    radio.receive((uint8_t*)&incomingPacket,
                  sizeof(incomingPacket));

  if(state == RADIOLIB_ERR_NONE) {

    Serial.println();
    Serial.println("REQUEST RECEIVED");

    // random delay to reduce collisions
    delay(random(100, 1500));

    // =========================================
    // SEND MULTIPLE RESPONSE PACKETS
    // =========================================

    const uint8_t TOTAL = 5;

    for(uint8_t i = 0; i < TOTAL; i++) {

      Packet response;

      response.senderId = MY_ID;

      response.packetNumber = i;
      response.totalPackets = TOTAL;

      response.timestamp = millis();

      response.value =
        random(100, 400) / 10.0;

      state =
        radio.transmit((uint8_t*)&response,
                       sizeof(response));

      if(state == RADIOLIB_ERR_NONE) {

        Serial.print("Sent packet ");
        Serial.print(i + 1);
        Serial.print("/");
        Serial.println(TOTAL);
      }
      else {

        Serial.print("Send failed: ");
        Serial.println(state);
      }

      // small gap between packets
      // delay(200);
    }

    Serial.println("All packets sent");
  }
}