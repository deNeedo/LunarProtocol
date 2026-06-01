#include "Blink.h"

void blink() {
  digitalWrite(LORA_LED, HIGH);
  delay(10);
  digitalWrite(LORA_LED, LOW);
}