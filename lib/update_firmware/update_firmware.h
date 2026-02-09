#ifndef LIB_UPDATE_FIRMWARE_H
#define LIB_UPDATE_FIRMWARE_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <memory>

/**
 * @brief OTA Firmware Update Manager for ESP8266
 * 
 * Handles checking for and installing firmware updates from GitHub Releases.
 * Uses modern C++ with smart pointers for automatic resource management.
 * 
 * Example usage:
 * @code
 * FirmwareUpdater updater;
 * 
 * void setup() {
 *   wifi_connect();
 *   updater.begin();  // Initialize SSL and sync time
 * }
 * 
 * void loop() {
 *   if (time_to_check_update) {
 *     updater.checkFirmware();  // Checks and installs if new version found
 *   }
 * }
 * @endcode
 */
class FirmwareUpdater {
public:
    FirmwareUpdater();
    ~FirmwareUpdater() = default;

    /**
     * @brief Initialize the firmware updater
     * 
     * Performs the following:
     * - Creates SSL certificate chain from embedded certs
     * - Initializes secure WiFi client
     * - Syncs system time via NTP (required for SSL validation)
     * - Sets trust anchors for GitHub API
     * 
     * Call this in setup() after WiFi is connected.
     */
    void begin();
    
    /**
     * @brief Check for firmware updates and install if available
     * 
     * Queries GitHub Releases, compares versions, and downloads/installs
     * new firmware if a newer version is detected. Device will reboot
     * automatically after successful update.
     */
    void checkFirmware();
    
    /**
     * @brief Download and install firmware from a specific URL
     * 
     * @param url Full URL to firmware binary (.bin file)
     * @return true if update was successful (device will reboot)
     * @return false if update failed
     * 
     * This method handles the entire OTA process including validation
     * and flashing. Use checkFirmware() for automatic version checking.
     */
    bool downloadFirmware(const String& url);

private:
    /**
     * @brief Sync system time via NTP
     * 
     * Waits for NTP time synchronization, which is required for
     * SSL certificate validation. Sets trust anchors after sync.
     */
    void syncTime();
    
    /**
     * @brief Follow HTTP redirects and return final URL
     * 
     * @param url Initial URL to check
     * @return String Final URL after following all redirects
     * 
     * GitHub redirects release downloads through their CDN.
     * This method follows those redirects to get the actual download URL.
     */
    String getUrlRedirect(const String& url);
    
    // Smart pointers for automatic resource management (RAII)
    std::unique_ptr<BearSSL::X509List> cert_;
    std::unique_ptr<BearSSL::WiFiClientSecure> client_;
};

#endif