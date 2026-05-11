#define TEST_PIN 48   // change this to the pin you want to test

void setup() {
  pinMode(TEST_PIN, OUTPUT);
}

void loop() {
  digitalWrite(TEST_PIN, HIGH);
  delay(100);

  digitalWrite(TEST_PIN, LOW);
  delay(100);
}