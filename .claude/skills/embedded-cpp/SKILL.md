---
name: embedded-cpp
description: C++ rules for embedded firmware (ESP32/Arduino) — fixed buffers, flash strings, no blocking in ISR/callbacks, sensor abstractions, non-blocking timers.
---

# Skill: embedded-cpp

C++ rules for embedded firmware development (ESP32/Arduino ecosystem). Consult before implementing or reviewing any firmware file.

## Memory

- No `malloc()` or `new` at runtime — heap fragmentation is fatal in embedded; allocate only in global constructors
- Use fixed-size buffers: `char buf[32]`, `char msg[255]` — size according to protocol/payload constraints
- Never use the Arduino `String` class — use `char[]` + `snprintf`

## Flash Strings (AVR only — do NOT use on ESP32)

- `F()`, `FPSTR()`, `PSTR()`, `snprintf_P`, and `_P`-suffix functions are AVR/PROGMEM artifacts
- On ESP32 they are no-ops or aliases — remove them and use plain string literals and `snprintf` directly

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