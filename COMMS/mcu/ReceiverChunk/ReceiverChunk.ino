#include <RadioLib.h>

SX1262 radio = new Module(41, 39, 42, 40);

const uint8_t MY_ID = 1;

void setup() {

  Serial.begin(115200);

  int state = radio.begin(875, 125, 7, 7);

  if(state != RADIOLIB_ERR_NONE) {

    Serial.println("Radio init failed!");

    while(true);
  }

  Serial.println("Rover ready");
}

void loop() {

  String received;

  // Wait for packet
  int state = radio.receive(received);

  if(state == RADIOLIB_ERR_NONE) {

    Serial.println();
    Serial.print("Received: ");
    Serial.println(received);

    // Check for PING command
    if(received.indexOf("PING") >= 0) {

      // Random backoff to avoid collisions
      delay(random(0, 100));

      Serial.println("PING detected -> sending PONGs");

      const uint8_t TOTAL = 5;

      for(uint8_t i = 0; i < TOTAL; i++) {

        // Build response message
        String response =
          "PONG;ID=" + String(MY_ID) +
          ";MSG=" + String(i + 1);

        state = radio.transmit(response);

        if(state == RADIOLIB_ERR_NONE) {

          Serial.print("Sent: ");
          Serial.println(response);
        }
        else {

          Serial.print("Send failed: ");
          Serial.println(state);
        }

        delay(100);
      }

      Serial.println("All PONG messages sent");
    }
  }
  else {

    Serial.print("Receive failed: ");
    Serial.println(state);
  }
}