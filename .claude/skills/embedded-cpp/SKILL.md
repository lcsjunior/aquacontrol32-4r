---
name: embedded-cpp
description: C++ rules for embedded firmware (ESP32/Arduino) — fixed buffers, flash strings, no blocking in ISR/callbacks, sensor abstractions, non-blocking timers.
---

# Skill: embedded-cpp

C++ rules for embedded firmware development (ESP32/Arduino ecosystem). Consult before implementing or reviewing any firmware file.

## Memory

- No `malloc()` or `new` at runtime — heap fragmentation is fatal in embedded; allocate only in global constructors
- Use fixed-size buffers: `char buf[32]`, `char msg[255]` — size according to protocol/payload constraints
- Never use the Arduino `String` class — use `char[]` + `snprintf` / `snprintf_P`

## Flash Strings

- `F()` in `Serial.print()` — keeps string literals in flash, not RAM
- `PSTR()` + `snprintf_P` for format strings with runtime arguments
- `FPSTR()` when passing PROGMEM strings to functions that accept `const __FlashStringHelper*`

## ISR / Callbacks

- Timer and interrupt callbacks must be minimal: toggle outputs or set flags only
- Never call `delay()`, `Serial`, or sensor reads inside an ISR or timer callback
- Do not invoke `Wire`, `SPI`, or `Serial` outside the main loop

## Sensor Access

- Access sensors only through their abstraction layer — never call driver methods directly from application code
- Assume throttling and timing are handled inside the abstraction; do not replicate that logic externally

## Output / Actuator Control

- Drive actuators only through the designated control layer (e.g., a thermostat or state machine handler)
- Direct actuator calls outside that layer must include a comment explaining the reason

## Non-Blocking Timers

- Use non-blocking timer patterns (elapsed-time checks against `millis()`) for all periodic tasks
- Never use `delay()` in `loop()` — it stalls WiFi, OTA, MQTT, and all cooperative tasks