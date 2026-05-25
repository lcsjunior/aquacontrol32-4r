# CLAUDE.md

## Skills → Actions

Consult the `SKILL.md` in `.claude/skills/<name>/` before implementing or reviewing.

| Skill                | Use for…                                                                      | Do not use for:                         |
|----------------------|--------------------------------------------------------------------------------|-----------------------------------------|
| `embedded-cpp`       | C++, memory allocation, ISR/callbacks, flash strings, relay, sensor           | tooling tasks, JSON configs, docs       |
| `platformio-build`   | Build, flash, monitor, `secrets.h`, dependencies in `platformio.ini`          | documentation-only changes              |

## Available Commands

| Command           | What it does                                                                    |
|-------------------|---------------------------------------------------------------------------------|
| `/new-feature`    | Full SDD flow: planner (PRD → TechSpec → Tasks) → implementer → reviewer        |

## SDD Agents

Generated documents saved in `tasks/prd-[slug]/`.

| Agent             | Role                                                           | When to use                                    |
|-------------------|----------------------------------------------------------------|------------------------------------------------|
| `@sdd-planner`    | PRD → TechSpec → Tasks (Explore + Plan, no production code)    | Start of any feature or fix                    |
| `@sdd-implementer`| Executes approved tasks, runs `pio run`, marks tasks complete  | After user approves tasks                      |
| `@sdd-reviewer`   | Reviews diff, runs `pio run`, applies embedded-cpp checklist   | After implementation, before commit            |

## Project Overview

ESP32 firmware for a smart aquarium controller running on a LILYGO T-Relay ESP32 board. Controls two planted nano aquariums via 4 relays: two heaters (K1, K2), LED bar (K3), and a CO2 solenoid valve (K4). Publishes sensor data to ThingSpeak via MQTT.

## Architecture

### Entry point
`src/main.cpp` wires everything together: initializes sensors/relays/thermostat, connects WiFi+MQTT, sets up cron schedules and an HTTP server, then runs the main loop.

### `lib/commons/` — reusable hardware abstractions
- **`relay.h/.cpp`** — `Relay` class: wraps a digital output pin, tracks on/off state, exposes `turnOn/turnOff/toggle`.
- **`temp_sensor.h/.cpp`** — `TempSensor` (abstract) and `DSTempSensor` (DS18B20 via OneWire/DallasTemperature). Throttles `requestTemperatures()` to every 300 ms.
- **`thermostat.h/.cpp`** — `Thermostat` class: state machine (`IDLE → COOLING → HEATING`) driven by temperature vs. setpoint±hysteresis. Takes a `Relay*` pointer and calls `turnOn/turnOff` accordingly. Enforces a 1-minute `IDLE_TIMEOUT` between state transitions.
- **`espx_wifi.h/.cpp`** — `WifiClass` singleton (`Wifi`): handles STA+AP mode, OTA updates, NTP sync, reconnect logic. Also provides helper utilities (`dBm2Quality`, `getLocalTimeFmt`, `mountFS`, etc.) and common PROGMEM string constants.

### `lib/config/` — persistent configuration
- **`config.h/.cpp`** — `Config` struct (`setpoint`, `hysteresis`) persisted to LittleFS as `/config.json`. Uses ArduinoJson custom converters (`convertToJson`/`convertFromJson`). Falls back to defaults (24°C setpoint, 0.5°C hysteresis) if no file exists.

### Cron schedules (hardcoded in `main.cpp`)
| Time  | Action           |
|-------|------------------|
| 07:30 | CO2 valve ON     |
| 08:00 | Lamp ON          |
| 14:30 | CO2 valve OFF    |
| 15:00 | Lamp OFF         |

### HTTP endpoints (port 80)
- `GET /` — health check
- `GET /reboot` — reboot device
- `GET /msg` — last MQTT publish payload
- `GET /lamp/toggle` — toggle lamp relay
- `GET /co2/toggle` — toggle CO2 valve relay

### MQTT
Connects to `mqtt3.thingspeak.com:1883`. Publishes ThingSpeak fields every 20 seconds: field1=temp1, field2=temp2, field3=heater1, field4=heater2, field5=lamp, field6=CO2 valve.

## Key Dependencies (platformio.ini)
- `ArduinoJson` 6.21.2 — config serialization
- `DallasTemperature` + `OneWire` — DS18B20 temperature sensors
- `CronAlarms` — cron-style scheduling
- `NoDelay` — non-blocking timers
- `PubSubClient` — MQTT client
