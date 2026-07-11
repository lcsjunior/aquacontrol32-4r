# CLAUDE.md

## Spec-Driven Development (SDD)

This project **strictly** follows a Spec-Driven Development flow driven by agents.
Artifacts for each feature live in `tasks/prd-<feature-name>/`.

## Overview

ESP32 firmware for a planted aquarium controller, on a LILYGO T-Relay board.
Hardware (pinout) and ThingSpeak field mapping: see `README.md`.

## Architecture (where things live)

- `src/main.cpp` — wiring only: initializes sensors, relays, and thermostat;
  calls `initWifi()`, `OTA.begin()`, `initHttpServer()`, `initCron()`; connects
  MQTT; owns the telemetry payload build and publish-on-interval
  (`buildPayload()`/`mqttPublish()`); runs the main loop.
- `src/wifi_setup` — `initWifi()`: brings up the `WiFiManager` captive portal
  (owns the `wifiManager` instance and its custom parameters for MQTT, cron,
  and thermostat config) and its save-parameters callback; configures NTP.
- `src/modules/http_server` — `initHttpServer()`: registers the `/health`,
  `/lamp/toggle`, `/co2/toggle`, `/heater/on` HTTP routes on
  `wifiManager.server`. `/lamp/toggle`, `/co2/toggle`, and `/heater/on`
  require HTTP Basic Auth (`requireAuth()`, credentials from the
  `WWW_USERNAME`/`WWW_PASSWORD` build-time macros); `/health` stays open.
  `/heater/on` calls `thermostat.forceTransition(&HeatingState)` and always
  replies `204 No Content`.
- `src/cron_setup` — `initCron()`: registers the four lamp/CO2 ON/OFF cron
  schedules, gating the ON crons on `safeCron`/`isClockSynced()`.
- `lib/commons/` — platform utilities: `clock.h` (`Clock` interface),
  `arduino_clock` (`ArduinoClockImpl` + extern `ArduinoClock` singleton; free
  functions `formatLocalDateTime()`, `isClockSynced()` / `isClockSynced(time_t)`,
  and the template `safeCron(obj, action)` that only invokes `action` on `obj`
  if the clock is synced), `utilities` (helpers: `getChipId`, `getApName`,
  `intToStr`, `floatToStr`, `loadingDelay`, `waitWifi`).
- `lib/devices/` — hardware abstractions: `Relay` (GPIO actuator), `DallasTemperatureSensor`
  (DS18B20), `Thermostat` + `ThermostatState` (state machine: `IdleState`,
  `HeatingState`; `forceTransition(ThermostatState*)` forces any target state,
  idempotent when already there), interfaces `Actuator` and `TemperatureSensor`.
  Also the pure free function `validateThermostatConfig` (safety rules for the
  persisted thermostat parameters; testable in `native`).
- `lib/mqtt/` — `MQTTClient` (publishes/subscribes to MQTT broker; configurable via
  `Config` or explicit parameters; auto-reconnect).
- `lib/ota/` — `OTAClient` + extern `OTA` singleton: wraps `ArduinoOTA`
  (`begin(hostname, password)` at setup, `handle()` every loop iteration),
  following the same singleton pattern as `lib/mqtt`.
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

<!-- SPECKIT START -->
Active plan: `specs/001-extract-main-modules/plan.md`. For additional
context about technologies to be used, project structure, shell commands,
and other important information for the feature in progress, read that plan.
<!-- SPECKIT END -->
