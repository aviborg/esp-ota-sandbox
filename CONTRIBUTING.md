# Contributing to ESP8266 OTA Update

Thank you for your interest in contributing! This document provides guidelines and instructions for contributing to this project.

## 🤝 How to Contribute

### Reporting Bugs
1. Check if the bug has already been reported in [Issues](../../issues)
2. If not, create a new issue using the Bug Report template
3. Provide as much detail as possible, including:
   - Serial output
   - Firmware version
   - Board type
   - Steps to reproduce

### Suggesting Features
1. Check if the feature has already been suggested
2. Create a new issue using the Feature Request template
3. Describe the use case and expected behavior

### Pull Requests
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Test thoroughly on actual hardware
5. Commit your changes (`git commit -m 'Add amazing feature'`)
6. Push to your branch (`git push origin feature/amazing-feature`)
7. Open a Pull Request using the PR template

## 🔧 Development Setup

### Prerequisites
- PlatformIO Core or PlatformIO IDE
- Python 3.x
- ESP8266 development board
- USB cable for programming

### Local Development
```bash
# Clone your fork
git clone https://github.com/YOUR_USERNAME/esp-ota-sandbox.git
cd esp-ota-sandbox

# Install dependencies
pip install platformio cryptography

# Build firmware
pio run -e esp12e

# Upload and monitor
pio run --target upload --target monitor
```

### Testing
Always test your changes on actual hardware before submitting a PR:
- Initial upload via serial
- WiFi configuration
- Update check mechanism
- OTA update process
- Post-update functionality

## 📝 Code Style

### C/C++ Code
- Use descriptive variable and function names
- Comment complex logic
- Keep functions focused and concise
- Follow existing code structure

### Python Scripts
- Follow PEP 8 style guide
- Add docstrings to functions
- Use meaningful variable names

### Commit Messages
- Use present tense ("Add feature" not "Added feature")
- Be descriptive but concise
- Reference issues when applicable

Examples:
```
✅ Add firmware size validation
✅ Fix OTA download timeout issue (#123)
✅ Update README with new configuration steps
❌ Fixed bug
❌ Update
```

## 🏗️ Project Structure

```
esp-ota-sandbox/
├── .github/              # GitHub workflows and templates
│   ├── workflows/        # CI/CD workflows
│   └── ISSUE_TEMPLATE/   # Issue templates
├── include/              # Header files
├── lib/                  # Library modules
│   ├── update_firmware/  # OTA update logic
│   └── wifi_connect/     # WiFi management
├── scripts/              # Build scripts
├── src/                  # Main source code
├── extra_scripts.py      # PlatformIO build hooks
└── platformio.ini        # PlatformIO configuration
```

## 🎯 Areas for Contribution

We welcome contributions in these areas:

### High Priority
- [ ] Firmware signature verification
- [ ] Rollback mechanism for failed updates
- [ ] Battery level checks for OTA
- [ ] Improved error reporting
- [ ] Unit tests

### Medium Priority
- [ ] Web interface for configuration
- [ ] MQTT integration for remote monitoring
- [ ] Multiple firmware channels (stable/beta)
- [ ] Custom update intervals
- [ ] Detailed logging system

### Low Priority
- [ ] mDNS support
- [ ] LED status indicators
- [ ] Additional board support
- [ ] Documentation translations

## ✅ Pull Request Review Process

1. **Automated Checks**: CI workflows must pass
   - Build successful
   - Firmware size within limits
   - No critical linting issues

2. **Code Review**: A maintainer will review your code
   - Code quality and style
   - Functionality
   - Documentation
   - Testing coverage

3. **Testing**: Changes should be tested on hardware
   - Initial functionality
   - OTA update process
   - Edge cases

4. **Merge**: Once approved, your PR will be merged

## 📊 Firmware Size Guidelines

- **Target**: < 500 KB (leaves plenty of OTA space)
- **Warning**: 500-900 KB (acceptable but monitor growth)
- **Critical**: > 900 KB (requires optimization)
- **Maximum**: 1 MB (ESP8266 limit for OTA)

Tips to reduce size:
- Disable debug symbols in production
- Remove unused libraries
- Optimize string storage
- Use PROGMEM for constants

## 🐛 Debugging

### Serial Output
Always enable serial debugging during development:
```cpp
Serial.begin(115200);
```

### HTTP Client Debugging
Enabled in `platformio.ini`:
```ini
-D DEBUG_ESP_HTTP_CLIENT
-D DEBUG_ESP_PORT=Serial
```

### Common Issues

**Build Errors**:
```bash
# Clean build
pio run --target clean
# Rebuild
pio run
```

**Upload Failures**:
- Check USB connection
- Verify correct COM port
- Try lower upload speed in platformio.ini

**OTA Issues**:
- Verify time sync is working
- Check SSL certificates
- Monitor free heap during download

## 🔐 Security

If you discover a security vulnerability, please email the maintainer directly rather than opening a public issue.

## 📜 License

By contributing, you agree that your contributions will be licensed under the same license as the project.

## 💬 Questions?

- Open a [Discussion](../../discussions)
- Join our community chat (if available)
- Tag maintainers in issues

## 🙏 Thank You!

Every contribution helps make this project better. Whether you're fixing a typo, reporting a bug, or adding a major feature, we appreciate your time and effort!
