#include <RadioLib.h>

SX1262 radio = new Module(41, 39, 42, 40);

#define MAX_MESSAGES 10
#define LISTEN_TIME 10000

const uint8_t MY_ID = 0;

// Structure for received messages
struct ReceivedMessage {

  uint8_t deviceId;
  String message;
};

// Array to store received packets
ReceivedMessage receivedMessages[MAX_MESSAGES];

int messageCount = 0;

void setup() {

  Serial.begin(115200);

  int state = radio.begin(875, 125, 7, 7);

  if(state != RADIOLIB_ERR_NONE) {

    Serial.println("Radio init failed!");

    while(true);
  }

  Serial.println("LoRa transmitter ready");
}

void loop() {

  // =====================================================
  // 1. SEND PING
  // =====================================================

  String pingMessage = "PING";

  int state = radio.transmit(pingMessage);

  if(state == RADIOLIB_ERR_NONE) {

    Serial.println();
    Serial.println("PING sent");
  }
  else {

    Serial.print("Send failed: ");
    Serial.println(state);

    delay(2000);
    return;
  }

  // =====================================================
  // 2. START LISTENING
  // =====================================================

  messageCount = 0;

  uint32_t listenStart = millis();

  Serial.println("Listening for responses...");

  while(millis() - listenStart < LISTEN_TIME) {

    String received;

    state = radio.receive(received);

    if(state == RADIOLIB_ERR_NONE) {

      Serial.print("Received: ");
      Serial.println(received);

      // ============================================
      // Parse format:
      // PONG;ID=2;MSG=1
      // ============================================

      int idStart = received.indexOf("ID=");
      int msgStart = received.indexOf(";MSG=");

      if(idStart >= 0 && msgStart >= 0) {

        String idText =
          received.substring(idStart + 3, msgStart);

        uint8_t deviceId = idText.toInt();

        // ignore own packets
        if(deviceId == MY_ID) {
          continue;
        }

        if(messageCount < MAX_MESSAGES) {

          receivedMessages[messageCount].deviceId =
            deviceId;

          receivedMessages[messageCount].message =
            received;

          messageCount++;

          // stop early if array full
          if(messageCount >= MAX_MESSAGES) {

            Serial.println("Max messages reached");
            break;
          }
        }
      }
    }
  }

  // =====================================================
  // 3. DISPLAY RESULTS
  // =====================================================

  Serial.println();
  Serial.println("=== RECEIVED MESSAGES ===");

  if(messageCount == 0) {

    Serial.println("No messages received");
  }

  for(int i = 0; i < messageCount; i++) {

    Serial.print("#");
    Serial.print(i + 1);

    Serial.print(" Device ID: ");
    Serial.print(receivedMessages[i].deviceId);

    Serial.print(" Content: ");
    Serial.println(receivedMessages[i].message);
  }

  Serial.println("=== END ===");
  Serial.println();

  delay(5000);
}