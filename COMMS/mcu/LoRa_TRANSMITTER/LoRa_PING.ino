#include <RadioLib.h>

// Pin definitions
#define LORA_CS   41
#define LORA_DIO1 39
#define LORA_RST  42
#define LORA_BUSY 40

#define LORA_LED  48
#define ESP_LED   21

#define DEVICE_ID 0

SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY);

uint32_t pingStart = 0;
bool waitingForAck = false;

void setup() {
  Serial.begin(115200);

  pinMode(USER_LED, INPUT);

  int state = radio.begin(868.0);

  if(state == RADIOLIB_ERR_NONE) {
    Serial.println("LoRa init OK");
  } else {
    Serial.println("LoRa init failed");
    while(true);
  }

  radio.startReceive();
}

void loop() {

  // BUTTON DETECT
  if(digitalRead(USER_LED) == HIGH && !waitingForAck) {

    String msg = "PING";

    pingStart = micros();

    radio.transmit(msg);

    waitingForAck = true;

    Serial.println("PING SENT");

    radio.startReceive();

    delay(300);
  }

  // RECEIVE
  String str;
  int state = radio.readData(str);

  if(state == RADIOLIB_ERR_NONE) {

    Serial.print("Received: ");
    Serial.println(str);

    // RECEIVED PING
    if(str == "PING") {

      uint32_t rxTime = micros();

      delayMicroseconds(500);

      uint32_t txTime = micros();

      uint32_t processingDelay = txTime - rxTime;

      String ack = "ACK:" + String(processingDelay);

      radio.transmit(ack);

      radio.startReceive();
    }

    // RECEIVED ACK
    if(str.startsWith("ACK:") && waitingForAck) {

      uint32_t pingEnd = micros();

      uint32_t totalRTT = pingEnd - pingStart;

      uint32_t remoteDelay =
        str.substring(4).toInt();

      float propTime =
        (totalRTT - remoteDelay) / 2.0;

      Serial.print("RTT: ");
      Serial.println(totalRTT);

      Serial.print("Remote delay: ");
      Serial.println(remoteDelay);

      Serial.print("Estimated propagation: ");
      Serial.println(propTime);

      waitingForAck = false;

      radio.startReceive();
    }
  }
}