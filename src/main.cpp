#include <Arduino.h>

#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

#include "wifi_connect.h"
#include "certs.h"

X509List cert(cert_DigiCert_High_Assurance_EV_Root_CA);
WiFiClientSecure client;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println(CLOUD_VERSION);
  Serial.println(CLOUD_DOWNLOAD_URL);
  wifi_connect();

    // Set time via NTP, as required for x.509 validation
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
  client.setTrustAnchors(&cert);
}

void loop() {
  static uint8_t led_state = 0;
  static unsigned int previous_check = -60000;

  digitalWrite(LED_BUILTIN, led_state);
  led_state ^= 1;
  delay(500);

  if(millis() - previous_check > 10000){
    const char * header_keys[] = {"location"};
    const size_t number_headers = 1;
    Serial.println("Check for update");
    previous_check = millis();
    HTTPClient http;
    String download_url(CLOUD_DOWNLOAD_URL);
    Serial.println(download_url);
    http.begin(client, download_url);
    http.collectHeaders(header_keys, number_headers);
    int http_code = http.sendRequest("HEAD", (const uint8_t*)nullptr, 0U);
    
    Serial.println(http_code);
    if (http_code >=300 && http_code < 400) {
      String location = http.header("location");
      Serial.print("location: ");
      Serial.println(location);
    }
    http.end();
  }


  yield();
}