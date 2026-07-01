#include <Arduino.h>

#define LED_PIN 2

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  for (int duty = 0; duty <= 100; duty++) {
    int on = duty / 5;     // 0-20ms
    int off = 20 - on;     // 20-0ms
    digitalWrite(LED_PIN, HIGH);
    delay(on);
    digitalWrite(LED_PIN, LOW);
    delay(off);
  }
  for (int duty = 100; duty >= 0; duty--) {
    int on = duty / 5;
    int off = 20 - on;
    digitalWrite(LED_PIN, HIGH);
    delay(on);
    digitalWrite(LED_PIN, LOW);
    delay(off);
  }
}
