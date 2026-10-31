#include <Arduino.h>

#define LED_PIN 2  // Vgrajena modra lučka

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("ESP32 je uspesno zagnan!");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("Pozdravljen svet iz ESP32!");
  delay(1000);

  digitalWrite(LED_PIN, LOW);
  delay(1000);
}