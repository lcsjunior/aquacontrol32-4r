# Project Conventions

Target is the **ESP32** (`esp32dev`, LILYGO T-Relay) plus the `native` host test
environment.

When a rule here conflicts with a hardware constraint (pinout, timing, memory),
the hardware wins.

## Memory

- Never use Arduino `String` — `char[]` + `snprintf`.
- Fixed-size buffers, sized from the protocol/payload.
- No `malloc()`/`new` at runtime; allocate in global constructors only.
- The Arduino loop task gets ~8 KB of stack: large buffers must be global or
  `static`, never locals.

## Constants

- Default to `constexpr` (`constexpr const char*`, `constexpr int`, …).
- `#define` (`UPPER_SNAKE_CASE`) only for pins and for timeouts/intervals, the
  latter with a `_MS` suffix and `UL` literal.

## Timing

- Never `delay()` in `loop()`; use `NoDelay` or elapsed-time checks against
  `millis()`.
- Keep `loop()` iterations short so the loop task yields and the watchdog stays
  fed.

## ISR / callbacks

- Handlers set flags or toggle outputs, nothing else. No `delay()`, `Serial`,
  or sensor reads.
- Mark them `IRAM_ATTR`.
- `Wire`, `SPI` and `Serial` only from the main loop.

## Hardware access

- Reach sensors and actuators through their abstraction layer (`lib/devices`),
  never the driver directly; assume throttling lives inside it.
- A direct call outside that layer needs a comment saying why.

## Naming

Arduino convention, which overrides any naming recommended in skills:

- `PascalCase` — classes and types.
- `camelCase` — methods, functions, variables, attributes.
- `UPPER_SNAKE_CASE` — macros and pins.
- Interfaces take the plain name (`TemperatureSensor`, no `I` prefix); when it
  collides with a concrete class, that class gets the `Impl` suffix.
- Global singletons are `PascalCase` like the Arduino core libs (`Serial`,
  `Wire`): `extern TelnetLogger TelnetLog;` in the header, defined at the top of
  the `.cpp`, right after the includes.

## Testability

- Singletons are for stateless utilities only. Logic covered by `native` tests
  takes its collaborators by interface through the constructor, so a fake can be
  injected.

## Style

- Code, identifiers and log strings in **English**.
- No comments — names must reveal intent (`heaterRelay`, not `obj`).
- Class sections in order: `public:` (constructor first), `protected:`,
  `private:` (attributes before methods).
- Prefer early return and the ternary over nested `if/else`.

## Logging

- Log via the ESP-IDF macros (`log_e`, `log_w`, `log_i`, `log_d`), never
  `Serial` directly. Exception: code that runs before WiFi and Telnet are up
  (e.g. `waitWifi()`) may use `Serial`.
- Never call a `log_*` macro from inside `TelnetLogger` (`lib/logger/`) —
  `log_printfv` is linker-wrapped into it, so that recurses forever.

## Dependencies

Pinned by version in `platformio.ini`. Do not add, remove or update one without
explicit approval.

## Documentation

Keep `README.md` (the product) and `CLAUDE.md` (how to work in the repo) current
in the same change set, without overlap between them.
