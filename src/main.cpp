#include <Arduino.h>
#include <cstdio>

void setup() {
  Serial.begin(115200);
  Serial.println(CLOUD_VERSION);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  static int wait_time = 1000;
  digitalWrite(LED_BUILTIN, HIGH);
  delay(wait_time);
  digitalWrite(LED_BUILTIN, LOW);
  delay(wait_time);
  wait_time = (wait_time + 100) % 1000;
  yield();
}