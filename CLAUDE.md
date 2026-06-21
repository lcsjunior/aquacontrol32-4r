# CLAUDE.md

## Spec-Driven Development (SDD)

This project **strictly** follows a Spec-Driven Development flow driven by agents.
Artifacts for each feature live in `tasks/prd-<feature-name>/`.

## Overview

ESP32 firmware for a planted aquarium controller, on a LILYGO T-Relay board.
Hardware (pinout) and ThingSpeak field mapping: see `README.md`.

## Architecture (where things live)

- `src/main.cpp` — general wiring: initializes sensors, relays, and thermostat; manages
  WiFi via `WiFiManager` (captive portal with custom parameters for MQTT, cron, and
  thermostat config, NTP) and OTA via `ArduinoOTA`; connects MQTT; registers crons
  and the HTTP server; runs the main loop.
- `lib/commons/` — platform utilities: `clock.h` (`Clock` interface),
  `arduino_clock` (`ArduinoClockImpl` + extern `ArduinoClock` singleton;
  exposes `formatLocalDateTime()` backed by an internal buffer), `utilities`
  (helpers: `dBmToQuality`, `getChipId`, `getApName`, `intToStr`, `floatToStr`).
- `lib/devices/` — hardware abstractions: `Relay` (GPIO actuator), `DallasTemperatureSensor`
  (DS18B20), `Thermostat` + `ThermostatState` (state machine: `IdleState`,
  `HeatingState`), interfaces `Actuator` and `TemperatureSensor`. Also the pure free
  function `validateThermostatConfig` (safety rules for the persisted thermostat
  parameters; testable in `native`).
- `lib/mqtt/` — `MQTTClient` (publishes/subscribes to MQTT broker; configurable via
  `Config` or explicit parameters; auto-reconnect).
- `lib/logger/` — `TelnetLogger` + extern `TelnetLog` singleton: streams ESP-IDF
  `log_*` output over TCP (port 23). Captured via linker wrap of `log_printfv`
  (see `platformio.ini`), so all `log_*` calls in the codebase reach Telnet
  clients transparently.
- `lib/fs/` — `Config` (OTA password, MQTT connection settings, 4 cron schedules,
  4 thermostat parameters: setpoint, hysteresis, lower/upper limit) persisted in
  LittleFS. Thermostat parameters are editable via the captive portal and validated
  by `validateThermostatConfig` before being persisted.

Details that change (cron schedules, HTTP endpoints, MQTT fields, lib versions)
live in the code (`src/main.cpp`) and `platformio.ini` — do not duplicate here.

## Conventions

**All code in this repository must follow `.claude/rules/code-conventions.md`.**
This file is the single source of truth for naming, class structure, logging,
constants, control flow, and the hardware-first tie-breaker rule. Read it
before writing or reviewing code — deviations break the consistency that
makes the codebase predictable across libs and reviewable in small diffs.
