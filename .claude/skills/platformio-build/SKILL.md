---
name: platformio-build
description: >
  PlatformIO build, flash, test, and configuration workflow for ESP32 —
  `pio run` (compile), `pio test -e native` (host tests), OTA and USB upload support.
allowed-tools: Bash, Read
---

# Skill: platformio-build

PlatformIO build, flash, and configuration workflow for this ESP32 project.

---

## Main Commands

```bash
# Compile/build code (works anywhere)
pio run

# Run all native (host) tests
pio test -e native

# Run a specific test runner
pio test -e native -f <runner_name>
```

---

## Build Commands

```bash
# Compile/build code (works anywhere)
pio run

# Upload to device
pio run --target upload

# Serial monitor at 115200 baud (requires physical USB connection)
pio device monitor

# Build + upload + monitor (convenient for local USB development)
pio run --target upload && pio device monitor
```

---

## Upload Methods

### Via OTA (Wi-Fi)

Requires `platformio.ini` configured with:

- `upload_protocol = espota`
- `upload_port` set to the device's IP address
- `upload_flags` with `--auth=<password>`

Works in remote sessions if the device is on the same network or accessible.

### Via USB

Default behavior when OTA is not configured. Requires the ESP32 to be physically connected via USB.

---

## Notes

- **Local session without device:** if compilation passes (`pio run` OK), the code is correct and ready to flash.
- **Upload requires user confirmation:** before running any upload/flash command (`pio run --target upload`, OTA or equivalent), always ask the user for explicit permission first.