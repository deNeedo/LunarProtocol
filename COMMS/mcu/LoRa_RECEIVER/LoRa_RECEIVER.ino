#include <RadioLib.h>

// Pin definitions
#define LORA_CS     41
#define LORA_DIO1   39
#define LORA_RST    42
#define LORA_BUSY   40
#define LORA_LED    48

SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY);

// LED blink variables
unsigned long ledOnTime = 0;
const int blinkDuration = 10; // LED ON time in ms
bool ledState = false;

// Last received message
String lastMessage = "";

void setup() {
  Serial.begin(115200);
  pinMode(LORA_LED, OUTPUT);
  digitalWrite(LORA_LED, LOW);

  delay(2000);
  Serial.println("Starting LoRa RX...");

  int state = radio.begin(868.0); // 868 MHz
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("Init failed, code: ");
    Serial.println(state);
    while (true);
  }

  Serial.println("LoRa init OK!");
  radio.startReceive();
}

void loop() {
  String received;
  int state = radio.readData(received);

  if (state == RADIOLIB_ERR_NONE) {
    received.trim();  // remove extra whitespace/newlines

    // Only print new messages
    if (received != lastMessage) {
      Serial.print("Received: ");
      Serial.println(received);

      lastMessage = received;

      // Start LED blink
      digitalWrite(LORA_LED, HIGH);
      ledOnTime = millis();
      ledState = true;
    }
  }
  else if (state != RADIOLIB_ERR_RX_TIMEOUT) {
    Serial.print("Error: ");
    Serial.println(state);
  }

  // Turn off LED after blinkDuration (non-blocking)
  if (ledState && millis() - ledOnTime >= blinkDuration) {
    digitalWrite(LORA_LED, LOW);
    ledState = false;
  }
}