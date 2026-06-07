# Project Conventions

## Language

- Code, identifiers, comments, and log strings: **English**.

## Naming

Follows the Arduino convention, which overrides the naming recommended in skills:

- `PascalCase` — classes/types (e.g.: `Relay`, `Thermostat`, `DSTempSensor`).
- `camelCase` — methods, functions, variables, and attributes (e.g.: `turnOn`, `getCTemp`).
- `UPPER_SNAKE_CASE` — macros and pin definitions (e.g.: `K1_PIN`, `DS18B20_PIN_1`).

### Interfaces (pure abstract classes)

- Use idiomatic C++ style: direct name **without `I` prefix** (e.g.: `TemperatureSensor`, `ThermostatState`).
- When the interface name collides with a concrete class, the **concrete class receives the `Impl` suffix** (e.g.: `ThermostatImpl`).

### Global singletons

Following the style of Arduino core libs (`Serial`, `Wire`, `SPI`, `EEPROM`), unique global
instances use **PascalCase** (not camelCase).

In all other cases, follow the conventions recommended in the skills.

## Class structure

Section order in every class declaration:

1. `public:` — constructor/destructor, then public methods.
2. `private:` — attributes, then private methods.

`protected:` follows the same logic when needed (between `public:` and `private:`).

## Dependencies

- Libraries are pinned by version in `platformio.ini`.
- Do not add, remove, or update dependencies without explicit approval.

## Code comments

- Do not add comments to code. Well-named identifiers are sufficient.
- Method, function, variable, and **object** names must be **descriptive**: reveal intent and behavior without requiring additional explanation.
- Object names must be suggestive of their role — `temperatureSensor` or `heaterRelay` instead of generic `obj` or `v`.

## Logging

- Use ESP-IDF macros (`log_i`, `log_w`, `log_e`, `log_d`) instead of `Serial.print`/`Serial.println`.
- Never use `Serial` directly for logging.

## Constants

- Constants in general should be declared as `constexpr const char*` (or equivalent type, e.g.: `constexpr int`), to guarantee compile-time evaluation and pointer/value immutability.
- Exceptions allowed as `#define` (`UPPER_SNAKE_CASE`):
  - **Pins** (e.g.: `#define K1_PIN 21`).
  - **Timeouts and time intervals**: `_MS` suffix and `UL` literal (e.g.: `#define MQTT_CONN_TIMEOUT_MS 5000UL`).

## Control flow

- Prefer early return and the ternary operator over nested `if/else` blocks.

## Hardware as source of truth

When a C++ guideline conflicts with a hardware constraint (pinout,
timing, ESP32 memory), the hardware constraint takes precedence.
