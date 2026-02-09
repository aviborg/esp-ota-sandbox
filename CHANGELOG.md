# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## v0.0.24

### Changed
- **BREAKING**: Refactored firmware update code into `FirmwareUpdater` class
  - Replaced procedural functions (`sync_time()`, `check_firmware()`, `download_firmware()`) with class methods
  - Renamed methods to camelCase: `begin()`, `checkFirmware()`, `downloadFirmware()`
  - Eliminated global variables (`cert`, `client`)
  - Implemented smart pointers (`std::unique_ptr`) for SSL client and certificate management
  - Based on ESP8266 Arduino BearSSL examples using modern C++ patterns
  
### Migration Guide

**Before:**
```cpp
#include "update_firmware.h"

void setup() {
  wifi_connect();
  sync_time();  // Global function
}

void loop() {
  check_firmware();  // Global function
}
```

**After:**
```cpp
#include "update_firmware.h"

FirmwareUpdater updater;  // Create instance

void setup() {
  wifi_connect();
  updater.begin();  // Class method
}

void loop() {
  updater.checkFirmware();  // Class method
}
```

### Added
- Smart pointer-based memory management with `std::unique_ptr`
- RAII pattern for SSL certificate and client lifecycle
- Comprehensive API documentation in README

### Improved
- Code organization with proper encapsulation
- Memory safety with automatic resource management
- Alignment with modern C++ best practices
