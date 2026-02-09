/*****
 * FirmwareUpdater - OTA Update Handler for ESP8266
 * 
 * A class-based implementation for checking and downloading firmware updates
 * from GitHub Releases. Uses modern C++ with smart pointers and RAII.
 * 
 * Features:
 * - Smart pointer-based memory management (std::unique_ptr)
 * - Automatic SSL certificate and client lifecycle management
 * - NTP time synchronization for SSL certificate validation
 * - HTTP redirect handling for GitHub CDN
 * - Streaming firmware download with OTA update
 * 
 * Usage:
 *   FirmwareUpdater updater;
 *   updater.begin();  // Initialize in setup()
 *   updater.checkFirmware();  // Check for updates in loop()
 * 
*/

#include "update_firmware.h"
#include "certs.h"

FirmwareUpdater::FirmwareUpdater() 
    : cert_(nullptr), client_(nullptr) {
}

void FirmwareUpdater::begin() {
    // Initialize certificate chain
    cert_ = std::make_unique<BearSSL::X509List>(github_cert);
    
    // Initialize secure WiFi client
    client_ = std::make_unique<BearSSL::WiFiClientSecure>();
    
    // Sync time and set trust anchors
    syncTime();
}

void FirmwareUpdater::syncTime() {
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
    
    client_->setTrustAnchors(cert_.get());
}

String FirmwareUpdater::getUrlRedirect(const String& url) {
    HTTPClient http;
    const char* header_keys[] = {"location"};
    const size_t number_headers = 1;
    int http_code = 300;
    String current_url = url;
    unsigned long check_timeout = millis();
    
    while (http_code >= 300 && http_code < 400 && (millis() - check_timeout) < 15000) {
        http.begin(*client_, current_url);
        http.collectHeaders(header_keys, number_headers);
        http.addHeader("Accept", "*/*");
        http_code = http.sendRequest("HEAD");
        
        if (http_code >= 300) {
            Serial.println("----");
            Serial.println(current_url);
            current_url = http.header("location");
            Serial.print(http_code);
            Serial.println(" Redirects to:");
            Serial.println(current_url);
            Serial.println("----");
        }
        http.end();
    }
    Serial.printf("HTTP code: %d\n", http_code);
    return current_url;
}

void FirmwareUpdater::checkFirmware() {
    Serial.println("Check for update");
    String download_url(CLOUD_DOWNLOAD_URL);
    download_url = getUrlRedirect(download_url);
    
    int idx = download_url.lastIndexOf('/');
    String tag = download_url.substring(idx + 1);
    
    if (tag.compareTo(CLOUD_VERSION) != 0) {
        Serial.print("New tag detected: ");
        Serial.println(tag);
        download_url = download_url.substring(0, idx - 1);
        idx = download_url.lastIndexOf('/');
        download_url = download_url.substring(0, idx);
        download_url += "/download/" + tag + "/firmware-" + tag + ".bin";
        Serial.println(download_url);
        
        downloadFirmware(download_url);
    }
}

bool FirmwareUpdater::downloadFirmware(const String& url) {
    HTTPClient http;
    Serial.printf("[HTTP] Download begin...\n");
    Serial.println(url);
    Serial.printf("URL length: %d chars\n", url.length());
    
    // Skip cert validation for CDN
    client_->setInsecure();
    http.begin(*client_, url);
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    Serial.print("[HTTP] GET...\n");
    ESP.wdtFeed();
    
    int httpCode = http.GET();
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            size_t contentLength = http.getSize();
            Serial.println("contentLength : " + String(contentLength));

            if (contentLength > 0) {
                // Initialize Update FIRST with U_FLASH parameter
                bool canBegin = Update.begin(contentLength, U_FLASH);
                if (!canBegin) {
                    Serial.println("Not enough space to begin OTA");
                    Serial.printf("Update error: %d\n", Update.getError());
                    client_->flush();
                    return false;
                }
                
                Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
                Serial.printf("Free sketch space: %u bytes\n", ESP.getFreeSketchSpace());
                
                // Get stream reference - don't copy it
                Stream& stream = http.getStream();
                size_t written = Update.writeStream(stream);
                http.end();
                client_->flush();
                
                if (written == contentLength) {
                    Serial.println("Written : " + String(written) + " successfully");
                } else {
                    Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
                    Serial.printf("Update error: %d\n", Update.getError());
                }

                if (Update.end()) {
                    Serial.println("OTA done!");
                    if (Update.isFinished()) {
                        Serial.println("Update successfully completed. Rebooting.");
                        ESP.restart();
                        return true;
                    } else {
                        Serial.println("Update not finished? Something went wrong!");
                        return false;
                    }
                } else {
                    Serial.println("Error Occurred. Error #: " + String(Update.getError()));
                    return false;
                }
            } else {
                Serial.println("There was no content in the response");
                client_->flush();
                return false;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
}