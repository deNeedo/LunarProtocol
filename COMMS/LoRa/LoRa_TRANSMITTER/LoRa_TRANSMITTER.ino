#include <RadioLib.h>

// Pin definitions
#define LORA_CS   41
#define LORA_DIO1 39
#define LORA_RST  42
#define LORA_BUSY 40

#define LORA_LED  48

SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY);

int counter = 0;

void setup() {
  pinMode(LORA_LED, OUTPUT);  // set pin as output
  Serial.begin(115200);
  delay(2000);

  Serial.println("Starting LoRa TX...");

  int state = radio.begin(868.0); // 868 MHz

  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("Init failed, code: ");
    Serial.println(state);
    while (true);
  }

  Serial.println("LoRa init OK!");
}

void loop() {
  String message = String(counter);

  Serial.print("Sending: ");
  Serial.println(message);

  int state = radio.transmit(message);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("Sent successfully!");
    digitalWrite(LORA_LED, HIGH); // turn ON
  } else {
    Serial.print("Send failed, code: ");
    Serial.println(state);
  }
  delay(10);
  digitalWrite(LORA_LED, LOW); // turn OFF after 10ms
  delay(90);
  counter++;
}