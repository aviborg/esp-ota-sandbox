/*****
 * 
 * 
 * 
 */

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>   


void wifi_connect()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    WiFiManager wifi_manager;
    WiFiManagerParameter version_text(CLOUD_VERSION);
    WiFiManagerParameter url_text(CLOUD_DOWNLOAD_URL);
    wifi_manager.addParameter(&version_text);
    wifi_manager.addParameter(&url_text);

    if (!wifi_manager.autoConnect()){
        Serial.println("failed to connect and hit timeout");
        ESP.restart();
        delay(1000);
    }
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}