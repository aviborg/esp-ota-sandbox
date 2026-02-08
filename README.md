# ESP8266 OTA Update from GitHub Releases

An ESP8266 firmware project that automatically checks for and installs updates from GitHub Releases. The device periodically checks for new firmware versions and performs Over-The-Air (OTA) updates without requiring a physical connection.

## Features

- ✅ **Automatic Update Checking** - Polls GitHub Releases every 24 hours
- ✅ **Secure Communication** - Uses NTP time sync and SSL/TLS with certificate validation for GitHub API
- ✅ **Version Tracking** - Compares current version with latest release tag
- ✅ **OTA Updates** - Downloads and installs firmware over WiFi
- ✅ **WiFiManager Integration** - Easy WiFi configuration via captive portal
- ✅ **Redirect Handling** - Properly follows GitHub's CDN redirects
- ✅ **Watchdog Management** - Prevents timeouts during long downloads

## Hardware Requirements

- ESP8266 board (ESP-12E, NodeMCU, Wemos D1 Mini, etc.)
- WiFi network connection
- USB cable for initial programming

## Software Requirements

- [PlatformIO](https://platformio.org/) (recommended) or Arduino IDE
- Python 3.x (for build scripts)

## Quick Start

### 1. Clone the Repository

```bash
git clone https://github.com/aviborg/esp-ota-sandbox.git
cd esp-ota-sandbox
```

### 2. Configure Version and Update URL

Set environment variables or edit `scripts/get_vars.py` defaults:

```bash
export CLOUD_VERSION="v0.0.1"
export CLOUD_DOWNLOAD_URL="https://github.com/YOUR_USERNAME/YOUR_REPO/releases/latest"
```

### 3. Build and Upload Initial Firmware

```bash
platformio run --target upload --target monitor
```

### 4. Configure WiFi

On first boot, the device creates a WiFi access point named `AutoConnectAP`. Connect to it and configure your WiFi credentials through the captive portal.

### 5. Create a GitHub Release

1. Build the firmware: `platformio run`
2. Find the binary at `.pio/build/esp12e/firmware.bin`
3. Create a new GitHub release with a version tag (e.g., `v0.0.2`)
4. Upload `firmware.bin` as `firmware-v0.0.2.bin` (matching the tag)

### 6. Wait for Automatic Update

The device will check for updates every 24 hours and automatically install new versions.

## Project Structure

```
esp-ota-sandbox/
├── include/
│   ├── certs.h           # Generated SSL certificates
│   └── README
├── lib/
│   ├── update_firmware/  # OTA update logic
│   │   ├── update_firmware.cpp
│   │   └── update_firmware.h
│   └── wifi_connect/     # WiFi management
│       ├── wifi_connect.cpp
│       └── wifi_connect.h
├── scripts/
│   ├── cert.py          # SSL certificate generator
│   └── get_vars.py      # Build-time configuration
├── src/
│   └── main.cpp         # Main application
├── extra_scripts.py     # PlatformIO build hooks
└── platformio.ini       # PlatformIO configuration
```

## How It Works

### Update Check Flow

1. **Initialization**
   - Device connects to WiFi using WiFiManager
   - Syncs time via NTP (required for SSL certificate validation)
   - Loads GitHub SSL certificates

2. **Version Check**
   - Every 24 hours, queries GitHub Releases API (via `CLOUD_DOWNLOAD_URL`)
   - Follows HTTP redirects to get latest release tag
   - Compares with current version (`CLOUD_VERSION`)

3. **Download & Install**
   - Constructs firmware URL: `https://github.com/user/repo/download/{TAG}/firmware-{TAG}.bin`
   - Downloads firmware using insecure SSL client (GitHub CDN uses different certificates)
   - Validates firmware format (checks for ESP8266 magic byte `0xE9`)
   - Writes to flash using `Update` library with `U_FLASH` flag
   - Reboots device with new firmware

### Key Technical Details

**Stream Handling**: The critical fix for OTA success is using a stream reference:
```cpp
Stream& stream = http.getStream();  // Reference, not copy
size_t written = Update.writeStream(stream);
```

**Dual SSL Clients**:
- Secure client for GitHub API (with certificate validation)
- Insecure client for CDN downloads (GitHub uses Azure CDN with different certs)

**Update Parameters**:
```cpp
Update.begin(contentLength, U_FLASH);  // U_FLASH is required!
```

## Configuration

### Environment Variables

Set these before building:

- `CLOUD_VERSION` - Current firmware version (e.g., `v0.0.1`)
- `CLOUD_DOWNLOAD_URL` - GitHub releases URL (e.g., `https://github.com/user/repo/releases/latest`)

### PlatformIO Configuration

Edit [platformio.ini](platformio.ini) to customize:

```ini
[env:esp12e]
platform = espressif8266
board = esp12e
framework = arduino
monitor_speed = 115200
```

### Build Flags

The project includes debug flags in `platformio.ini`:
```ini
-D DEBUG_ESP_HTTP_CLIENT  # Enables detailed HTTP client logging
-D DEBUG_ESP_PORT=Serial  # Directs debug output to Serial
```

## Building Firmware

### Development Build

```bash
platformio run
```

Binary location: `.pio/build/esp12e/firmware.bin`

### Upload via Serial

```bash
platformio run --target upload
```

### Monitor Serial Output

```bash
platformio run --target monitor
```

### Combined Upload & Monitor

```bash
platformio run --target upload --target monitor
```

## Creating a Release

The GitHub Actions workflow automates the entire release process:

1. **Create Release on GitHub**:
   - Go to GitHub → Releases → "Draft a new release"
   - Create a new tag (e.g., `v0.0.2`)
   - Set title: `Release v0.0.2`
   - Add release notes (optional)
   - Click "Publish release"

2. **Automated Build**: GitHub Actions automatically:
   - Builds the firmware with PlatformIO
   - Uploads `firmware-v0.0.2.bin` to your release
   - Generates and uploads checksums (SHA256, MD5)

3. **Device Updates**: Your ESP8266 devices will detect and install the update within 24 hours

> **Note**: The `CLOUD_VERSION` environment variable is automatically extracted from the release tag during the build process.

## CI/CD Workflows

This project includes automated GitHub Actions workflows for continuous integration and deployment:

### 🚀 Release Workflow (`.github/workflows/release.yml`)

**Trigger**: Create a release through GitHub UI or API

**What it does**:
- ✅ Builds firmware with PlatformIO
- ✅ Generates SHA256 and MD5 checksums
- ✅ Uploads firmware binary with version name (e.g., `firmware-v1.0.0.bin`)
- ✅ Includes checksums for security verification
- ✅ Reports firmware size and build information

**Usage**:
1. Go to GitHub repository → Releases → "Draft a new release"
2. Create a tag (e.g., `v1.0.0`) and set as release title
3. Add release notes (optional)
4. Click "Publish release"

The workflow automatically uploads to your release:
- `firmware-v1.0.0.bin` - The firmware binary
- `firmware-v1.0.0.bin.sha256` - SHA256 checksum
- `firmware-v1.0.0.bin.md5` - MD5 checksum

### 🔨 CI Workflow (`.github/workflows/ci.yml`)

**Trigger**: Push to `main`/`master`/`develop` branches or pull requests

**What it does**:
- ✅ Builds firmware for verification
- ✅ Runs PlatformIO static analysis checks
- ✅ Checks firmware size limits (max 1 MB for ESP8266)
- ✅ Uploads build artifacts for review
- ✅ Provides build summaries in PR comments
- ✅ Lints Python scripts

**Features**:
- Matrix builds for multiple configurations
- Artifacts retained for 30 days
- Firmware size warnings at 900 KB
- Build fails if firmware exceeds 1 MB

### 🔒 Security Workflow (`.github/workflows/security.yml`)

**Trigger**: Weekly (Sundays) or manual dispatch

**What it does**:
- ✅ Checks for outdated dependencies
- ✅ Runs security audit on Python packages
- ✅ Validates PlatformIO library versions
- ✅ Performs firmware health check
- ✅ Verifies SSL certificate validity

**Reports**:
- Lists outdated packages
- Security vulnerabilities in dependencies
- Firmware size trends
- Certificate expiration status

### Workflow Badges

Add these badges to your README to show build status:

```markdown
![Release](https://github.com/YOUR_USERNAME/YOUR_REPO/actions/workflows/release.yml/badge.svg)
![CI](https://github.com/YOUR_USERNAME/YOUR_REPO/actions/workflows/ci.yml/badge.svg)
![Security](https://github.com/YOUR_USERNAME/YOUR_REPO/actions/workflows/security.yml/badge.svg)
```

## Automated Release Process

### Standard Release
```bash
# 1. Update version and commit changes
git add .
git commit -m "Release v1.0.0"

# 2. Create and push tag
git tag v1.0.0
git push origin v1.0.0

# 3. Workflow automatically:
#    - Builds firmware
#    - Creates release
#    - Uploads assets
```

### Pre-release / Beta
```bash
git tag v1.0.0-beta.1
git push origin v1.0.0-beta.1
```

The workflow will mark tags with `-beta`, `-alpha`, or `-rc` as pre-releases.

## Troubleshooting

### OTA Update Fails

**Error 10 (UPDATE_ERROR_NO_PARTITION)**
- Missing `U_FLASH` parameter in `Update.begin()`
- Fix: `Update.begin(contentLength, U_FLASH);`

**Error 13 (UPDATE_ERROR_MAGIC_BYTE)**
- Firmware file is not a valid ESP8266 binary
- Verify you uploaded the correct `.bin` file from PlatformIO build

**Connection Closes Before Download**
- Using stream copy instead of reference
- Fix: `Stream& stream = http.getStream();` (not `WiFiClient stream = ...`)

### WiFi Connection Issues

- Reset WiFi settings: Hold reset button during boot
- The device will create `AutoConnectAP` access point
- Connect and reconfigure

### Certificate Errors

Certificates are auto-generated during build. To regenerate:
```bash
python scripts/cert.py -s api.github.com -n github > include/certs.h
```

### Time Sync Fails

- Ensure NTP servers are accessible: `pool.ntp.org`, `time.nist.gov`
- Check firewall settings (NTP uses UDP port 123)

### Debug Output

Enable verbose HTTP debugging in `platformio.ini`:
```ini
-D DEBUG_ESP_HTTP_CLIENT
-D DEBUG_ESP_PORT=Serial
```

## Serial Output Example

Successful update:
```
Check for update
HTTP code: 200
New tag detected: v0.0.21
https://github.com/aviborg/esp-ota-sandbox/releases/download/v0.0.21/firmware-v0.0.21.bin
[HTTP] Download begin...
URL length: 89 chars
[HTTP] GET...
[HTTP] GET... code: 200
contentLength : 444512
Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!
Free sketch space: 2666496 bytes
Written : 444512 successfully
OTA done!
Update successfully completed. Rebooting.
```

## Dependencies

Managed automatically by PlatformIO:

- **ESP8266 Arduino Core** - ESP8266WiFi, ESP8266HTTPClient
- **WiFiManager** (v2.0.17) - WiFi configuration portal
- **Python packages** (build-time):
  - `cryptography` - SSL certificate handling

## License

See [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Acknowledgments

- ESP8266 Arduino Core team
- WiFiManager by tzapu
- GitHub for hosting releases

## Author

aviborg

---

**Note**: This is a sandbox/example project demonstrating OTA updates from GitHub Releases. For production use, consider adding:
- Firmware signature verification
- Rollback mechanism
- Update size validation before download
- Configurable update check intervals
- Battery level checks (for battery-powered devices)
