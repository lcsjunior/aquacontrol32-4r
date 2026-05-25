---
name: embedded-cpp
description: C++ rules for ESP32 embedded firmware — no malloc outside global constructors, flash strings with F()/PSTR(), no blocking in ISR/callbacks, fixed buffers, sensors via abstractions.
---

# Skill: embedded-cpp

C++ rules for ESP32 embedded firmware. Consult before implementing or reviewing any file in `src/` or `lib/`.

## Memory

- No `malloc()` or `new` outside global object constructors — heap fragmentation is fatal in embedded
- Use fixed buffers: `char topic[32]`, `char msg[255]` (see main.cpp as reference)
- Never use the Arduino `String` class — use `char[]` + `snprintf_P`

## Flash Strings

- `F()` in `Serial.print()` — keeps literals in flash, not RAM
- `PSTR()` + `snprintf_P` for format strings with arguments (see `writeMsg()` in main.cpp)
- Prefer `FPSTR()` when passing PROGMEM strings to functions that accept `const __FlashStringHelper*`

## ISR / Callbacks

- Cron callbacks: ONLY `relay.turnOn()` / `relay.turnOff()` — no `delay()`, no `Serial`, no sensor reads
- Never block in an interrupt context or timer callback
- Do not call `Wire`, `SPI`, or `Serial` outside the main loop

## Sensors

- DS18B20 always via `DSTempSensor::getCTemp()` — never access `DallasTemperature` directly
- `requestTemperatures()` is throttled internally in the class — do not call it externally

## Relay

- Relays driven only by `Thermostat::handleHeater()` or HTTP handlers registered in `initWS()`
- Direct calls to `relay.turnOn/Off()` outside those points require a comment justifying the reason

## Non-Blocking Timers

- Use `noDelay` for timeouts (already present: `mqttConnTime`, `mqttPubTime`)
- Never use `delay()` in `loop()` — it stalls WiFi, OTA, and MQTT
