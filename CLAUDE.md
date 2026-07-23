# CLAUDE.md

Guidance for Claude Code (claude.ai/code) when working in this repository.

## Overview

ESP32 firmware (`esp32dev`, LILYGO T-Relay board) for a planted aquarium
controller: DS18B20 thermostat, cron-scheduled lamp and CO2, MQTT telemetry to
ThingSpeak. Pinout, ThingSpeak fields and HTTP endpoints — see `README.md`.

## Architecture (where things live)

- `src/main.cpp` — wiring only: devices, `init*()` calls, MQTT publish, `loop()`.
- `src/modules/` — application wiring: `wifi_setup` (captive portal, custom
  fields, save callback, NTP), `ota_setup`, `http_server` (routes on
  `wifiManager.server`), `cron_setup` (lamp/CO2 schedules).
- `lib/devices/` — hardware abstractions: `Relay`, `DallasTemperatureSensor`,
  `Thermostat` + its states, behind the `Actuator`/`TemperatureSensor`
  interfaces so they can be faked in `native` tests.
- `lib/fs/` — `AppConfig`: LittleFS + ArduinoJson persistence of `/config.json`.
- `lib/mqtt/`, `lib/logger/` — `MQTT` broker client and `TelnetLog`, a Telnet
  sink for `log_*` output (port 23, via the `log_printfv` linker wrap in
  `platformio.ini`).
- `lib/commons/` — helpers shared across modules: clock, formatting, WiFi waits.

Anything user-configurable belongs in the portal plus `Config`, not in build
flags. Details that change (cron schedules, endpoints, MQTT fields, lib
versions) live in the code and `platformio.ini` — do not duplicate here.

## Build, Test, Upload

```bash
pio run                 # compile
pio test -e native      # host tests
pio run --target upload # flash
pio device monitor      # serial monitor @ 115200 baud
```

- **Always ask for explicit user confirmation before any upload/flash** — it
  writes to physical hardware.
- The upload path is OTA (`espota`) via the untracked `local_settings.ini`;
  removing it falls back to USB.

## Spec-Driven Development (SDD)

This project **strictly** follows an agent-driven SDD flow. Artifacts for each
feature live in `tasks/prd-<feature-name>/`.

## Conventions

**All code must follow `.claude/rules/code-conventions.md`** — the single source
of truth for naming, class structure, logging, constants, control flow, and the
hardware-first tie-breaker rule. Read it before writing or reviewing.
