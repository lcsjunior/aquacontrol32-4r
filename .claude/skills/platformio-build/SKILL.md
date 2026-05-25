---
name: platformio-build
description: PlatformIO build, flash, and configuration workflow for ESP32 — pio run (compile), secrets.h required, dependencies in platformio.ini, upload/monitor require physical USB.
---

# Skill: platformio-build

PlatformIO build, flash, and configuration workflow for this ESP32 project.

## Main Commands

```bash
# Compile (works in a remote session)
pio run

# Upload to device (requires physical USB — does not work in a remote session)
pio run --target upload

# Serial monitor at 115200 baud (requires physical USB — does not work in a remote session)
pio device monitor

# Build + upload + monitor in one step
pio run --target upload && pio device monitor
```

## secrets.h

`src/secrets.h` is in `.gitignore` and must exist locally before building. Verify all defines are present:

- `WIFI_SSID`, `WIFI_PASS`
- `OTA_PASS`, `AP_PASS`
- `MQTT_CLIENT_ID`, `MQTT_USERNAME`, `MQTT_PASSWORD`
- `SMART_AQUARIUM_WR_CH_ID`

**Never display the values** — only confirm the presence of each `#define`. If the file is missing, the build fails with an include error.

## Dependencies

Managed in `platformio.ini`. Do not change library versions without checking breaking changes in each lib's changelog.

| Library            | Version | Usage                      |
|--------------------|---------|----------------------------|
| ArduinoJson        | 6.21.2  | Config serialization        |
| DallasTemperature  | 3.11.0  | DS18B20 sensor              |
| OneWire            | 2.3.7   | OneWire protocol            |
| CronAlarms         | 0.1.0   | Cron-style scheduling       |
| NoDelay            | 2.1.1   | Non-blocking timers         |
| PubSubClient       | 2.8     | MQTT client                 |

**Guideline for local session without device:** If compilation passes (`pio run` OK), the code is correct and ready. Report build success and indicate that upload/monitor require ESP32 connected via USB.
