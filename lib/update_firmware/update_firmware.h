#ifndef LIB_UPDATE_FIRMWARE_H
#define LIB_UPDATE_FIRMWARE_H
#include "Arduino.h"

void sync_time();

void check_firmware();

bool download_firmware(String url);

#endif