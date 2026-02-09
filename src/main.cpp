#include <Arduino.h>
#include <ESP8266HTTPClient.h>

#include "wifi_connect.h"
#include "update_firmware.h"

FirmwareUpdater updater;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.printf("Current version: %s\n", CLOUD_VERSION);
  Serial.printf("Download url: %s\n", CLOUD_DOWNLOAD_URL);
  wifi_connect();
  updater.begin();
}

void loop() {
  const unsigned long update_interval = 24 * 3600 * 1000;
  static unsigned long previous_check = -update_interval;

  static uint8_t led_state = 0;
  digitalWrite(LED_BUILTIN, led_state);
  led_state ^= 1;
  delay(500);

  if(millis() - previous_check > update_interval){
    previous_check = millis();
    updater.checkFirmware();
  }

  yield();
}