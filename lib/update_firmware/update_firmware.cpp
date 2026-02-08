/*****
 * 
 * Check for and download new firmware
 * 
*/

#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

#include "update_firmware.h"
#include "certs.h"

X509List cert(github_cert);
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

String get_url_redirect(String url) {
  HTTPClient http;
  const char * header_keys[] = {"location"};
  const size_t number_headers = 1;
  int http_code = 300;
  unsigned long check_timeout = millis();
  while (http_code >=300 && http_code < 400 && (millis()-check_timeout) < 15000) {
    http.begin(client, url);
    http.collectHeaders(header_keys, number_headers);
    http.addHeader("Accept", "*/*");
    http_code = http.sendRequest("HEAD");
    if(http_code >= 300) {
      Serial.println("----");
      Serial.println(url);
      url = http.header("location");
      Serial.print(http_code);
      Serial.println(" Redirects to:");
      Serial.println(url);
      Serial.println("----");
    }
    http.end();
  }
  Serial.printf("HTTP code: %d\n", http_code);
  return url;
}

void check_firmware()
{
    Serial.println("Check for update");
    String download_url(CLOUD_DOWNLOAD_URL);
    download_url = get_url_redirect(download_url);
    
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

bool download_firmware(String url)
{
  // Create a separate insecure client for CDN downloads
  WiFiClientSecure downloadClient;
  downloadClient.setInsecure(); // Skip cert validation for CDN
  //downloadClient.setBufferSizes(1024, 1024);
  
  HTTPClient http;
  Serial.printf("[HTTP] Download begin...\n");
  Serial.println(url);
  Serial.printf("URL length: %d chars\n", url.length());
  
  http.begin(downloadClient, url);
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  int httpCode = http.GET();


  //http.setReuse(false);
  //http.setTimeout(60000);
  

  Serial.print("[HTTP] GET...\n");
  ESP.wdtFeed();
  
  
  
  if (httpCode > 0)
  {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK)
    {
      size_t contentLength = http.getSize();
      Serial.println("contentLength : " + String(contentLength));

      if (contentLength > 0)
      {
        // Initialize Update FIRST with U_FLASH parameter
        bool canBegin = Update.begin(contentLength, U_FLASH);
        if (!canBegin)
        {
          Serial.println("Not enough space to begin OTA");
          Serial.printf("Update error: %d\n", Update.getError());
          client.flush();
          return false;
        }
        
        Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
        Serial.printf("Free sketch space: %u bytes\n", ESP.getFreeSketchSpace());
        
        // Get stream reference - don't copy it
        Stream& stream = http.getStream();
        size_t written = Update.writeStream(stream);                
        
        if (written == contentLength) {
          Serial.println("Written : " + String(written) + " successfully");
        } else {
          Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
          Serial.printf("Update error: %d\n", Update.getError());
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