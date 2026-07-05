#include <Arduino.h>
#include "esp_rom_sys.h"
#define LED_PIN 2

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  esp_rom_delay_us(100000);
  digitalWrite(LED_PIN, LOW);
  esp_rom_delay_us(100000);
}
