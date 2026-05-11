#include <RadioLib.h>

// Pin definitions
#define LORA_CS   41
#define LORA_DIO1 39
#define LORA_RST  42
#define LORA_BUSY 40

#define LORA_LED  48
#define USER_LED  21

#define DEVICE_ID 1

SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY);

uint32_t pingStart = 0;
uint32_t pingLast = 0;
uint32_t timeNow = 0;
uint32_t pingEnd = 0;
bool waitingForAck = false;

struct AckPacket {
    uint8_t  type;
    uint32_t remoteDelay;
};

void blink() {
  digitalWrite(LORA_LED, HIGH);
  delay(10);
  digitalWrite(LORA_LED, LOW);
}

void setup() {
  Serial.begin(115200);

  // pinMode(USER_LED, INPUT_PULLUP);
  pinMode(LORA_LED, OUTPUT);  // set pin as output

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
  if(millis() - pingLast > 10000 && !waitingForAck && DEVICE_ID == 0) {
    AckPacket ping;
    pingStart = millis();
    ping.type = 1; ping.remoteDelay = pingStart;
    radio.transmit((uint8_t*)&ping, sizeof(ping));
    blink();
    waitingForAck = true;
    pingLast = pingStart;
    Serial.print("PING SENT: ");
    Serial.println(pingLast);
    radio.startReceive();
  }

  // RECEIVE
  AckPacket ack;
  int state = radio.readData((uint8_t*)&ack, sizeof(ack));

  if(state == RADIOLIB_ERR_NONE) {
    // RECEIVED PING
    if(ack.type == 1) {
      Serial.print("PING RECEIVED: ");
      Serial.println(ack.remoteDelay);
      uint32_t rxTime = millis();
      blink();
      uint32_t txTime = millis();
      uint32_t processingDelay = txTime - rxTime;
      ack.type = 2;
      ack.remoteDelay = processingDelay;
      radio.transmit((uint8_t*)&ack, sizeof(ack));
      radio.startReceive();
    }

    // RECEIVED ACK
    if(ack.type == 2 && waitingForAck) {
      Serial.println("ACK RECEIVED");
      pingEnd = millis();
      blink();
      uint32_t totalRTT = pingEnd - pingStart;
      uint32_t remoteDelay = ack.remoteDelay;
      float propTime = (totalRTT - remoteDelay) / 2.0;

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