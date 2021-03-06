/*****
 * 
 * Check for and download new firmware
 * 
*/

#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

#include "update_firmware.h"
#include "certs.h"

X509List cert(cert_DigiCert_High_Assurance_EV_Root_CA);
WiFiClientSecure client;

void sync_time(){
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

void check_firmware()
{
    
    const char * header_keys[] = {"location"};
    const size_t number_headers = 1;
    Serial.println("Check for update");
    
    HTTPClient http;
    String download_url(CLOUD_DOWNLOAD_URL);
    int http_code = 300;
    unsigned long check_timeout = millis();
    while (http_code >=300 && http_code < 400 && (millis()-check_timeout) < 15000) {
      Serial.println(download_url);
      http.begin(client, download_url);
      http.collectHeaders(header_keys, number_headers);
      http_code = http.sendRequest("HEAD", (const uint8_t*)nullptr, 0U);
      if(http_code >= 300) {
        download_url = http.header("location");
        Serial.println("Redirects to:");
      }
      http.end();
    }
    Serial.printf("HTTP code: %d\n", http_code);
    if (http_code >=200 && http_code < 300) {
      int idx = download_url.lastIndexOf('/');
      String tag = download_url.substring(idx + 1);
      if(tag.compareTo(CLOUD_VERSION) != 0) {
        Serial.print("New tag detected: ");
        Serial.println(tag);
        download_url = download_url.substring(0, idx - 1);
        idx = download_url.lastIndexOf('/');
        download_url = download_url.substring(0, idx);
        download_url += "/download/" + tag + "/firmware-" + tag + ".bin";
        Serial.println(download_url);
        download_firmware(download_url);
      }
    }
}

/* 
 * Download binary image and use Update library to update the device.
 */
bool download_firmware(String url)
{
  HTTPClient http;
  Serial.print("[HTTP] Download begin...\n");

  http.begin(client, url);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  if (httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK)
    {

      int contentLength = http.getSize();
      Serial.println("contentLength : " + String(contentLength));

      if (contentLength > 0)
      {
        bool canBegin = Update.begin(contentLength);
        if (canBegin)
        {
          // client = http.getStream();
          Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
          size_t written = Update.writeStream(client);

          if (written == static_cast<size_t>(contentLength))
          {
            Serial.println("Written : " + String(written) + " successfully");
          }
          else
          {
            Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
          }

          if (Update.end())
          {
            Serial.println("OTA done!");
            if (Update.isFinished())
            {
              Serial.println("Update successfully completed. Rebooting.");
              ESP.restart();
              return true;
            }
            else
            {
              Serial.println("Update not finished? Something went wrong!");
              return false;
            }
          }
          else
          {
            Serial.println("Error Occurred. Error #: " + String(Update.getError()));
            return false;
          }
        }
        else
        {
          Serial.println("Not enough space to begin OTA");
          client.flush();
          return false;
        }
      }
      else
      {
        Serial.println("There was no content in the response");
        client.flush();
        return false;
      }
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}