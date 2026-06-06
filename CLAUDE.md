# CLAUDE.md

## Spec-Driven Development (SDD)

This project **strictly** follows a Spec-Driven Development flow driven by agents.
Artifacts for each feature live in `tasks/prd-<feature-name>/`.

## Overview

ESP32 firmware for a planted aquarium controller, on a LILYGO T-Relay board.
Hardware (pinout) and ThingSpeak field mapping: see `README.md`.

## Architecture (where things live)

- `src/main.cpp` — general wiring: initializes sensors, relays, and thermostat; connects
  WiFi+MQTT; registers crons and the HTTP server; runs the main loop.
- `lib/commons/` — platform utilities: `clock.h` (`Clock` interface),
  `arduino_clock` (concrete impl).
- `lib/wifi/` — `WiFiLib` (STA+AP, OTA, NTP); global singleton `WIFI`.
- `lib/devices/` — hardware abstractions: `Relay` (GPIO actuator), `DallasTemperatureSensor`
  (DS18B20), `Thermostat` + `ThermostatState` (state machine: `IdleState`,
  `HeatingState`), interfaces `Actuator` and `TemperatureSensor`.
- `lib/mqtt/` — `MQTTClient` (data publishing to MQTT broker).
- `lib/fs/` — `Config` (setpoint, hysteresis) persisted in LittleFS.

Details that change (cron schedules, HTTP endpoints, MQTT fields, lib versions)
live in the code (`src/main.cpp`) and `platformio.ini` — do not duplicate here.

## Conventions

Follow all rules defined in `.claude/rules/`.
