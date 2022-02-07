/*****
 * 
 * Check for and download new firmware
 * 
*/

#include <ESP8266HTTPClient.h>

void download_firmware()
{
    HTTPClient http;
    String downloadUrl(CLOUD_DOWNLOAD_URL);

}


void update_firmware()
{
    download_firmware();
}