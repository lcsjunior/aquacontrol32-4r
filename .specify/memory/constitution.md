<!--
Sync Impact Report
==================
Version change: [TEMPLATE] → 1.0.0 (initial ratification)
Modified principles: n/a (first concrete version, template placeholders replaced)
Added sections:
  - Core Principles I-V (Hardware-First Truth, Non-Blocking Main Loop,
    Fail-Safe Thermostat Safety, Hardware Abstraction & Native Testability,
    Convention Consistency)
  - Technology Stack & Architecture Boundaries
  - Development Workflow
  - Governance
Removed sections: none (template scaffold only)
Templates requiring updates:
  - .specify/templates/plan-template.md ✅ (Constitution Check gate is generic,
    reads this file at runtime — no edit needed)
  - .specify/templates/spec-template.md ✅ (no principle-specific references)
  - .specify/templates/tasks-template.md ✅ (generic phase structure, no
    principle-specific references)
  - .specify/extensions/agent-context/** ✅ (agent-context sync is generic)
  - CLAUDE.md ✅ (already documents architecture/conventions; constitution
    defers to it rather than duplicating)
Follow-up TODOs: none
-->

# AquaControl32-4R Constitution

## Core Principles

### I. Hardware-First Truth (NON-NEGOTIABLE)

When a software convention conflicts with a hardware constraint — pinout,
timing, or ESP32 flash/RAM/CPU budget — the hardware constraint MUST win.
Pin assignments, timeouts, and the flash partition layout (`min_spiffs.csv`)
are physical facts, not implementation details to refactor away for the sake
of elegance. This mirrors the "Hardware as source of truth" rule already
codified in `.claude/rules/code-conventions.md`; the constitution elevates it
to a project-wide gate that applies to architecture decisions, not only to
line-level code style.

**Rationale**: This is firmware controlling a live aquarium (heater, CO2
solenoid, lighting). A design that is "cleaner" in software terms but ignores
a real timing or memory constraint can brick the device or, worse, leave a
relay in the wrong state.

### II. Non-Blocking Main Loop (NON-NEGOTIABLE)

`loop()` and everything reachable from it MUST NOT block. Every subsystem is
driven by cooperative polling inside a single `loop()` pass: `wifiManager.process()`,
`Cron.delay()`, `TelnetLog.loop()`, `ArduinoOTA.handle()`, `MQTT.loop()`, and
`noDelay`-based interval checks. Blocking calls (`delay()`, long synchronous
I/O) are permitted only in one-time boot sequences before `loop()` starts
(e.g., `waitWifi`, `loadingDelay`) — never inside `loop()`, a cron callback, or
an MQTT/HTTP handler.

**Rationale**: A blocked loop stalls the thermostat's `update()` call, WiFi
management, OTA, and MQTT publishing simultaneously — there is no scheduler to
fall back on. Cooperative, non-blocking design is what keeps temperature
control responsive on a single-threaded Arduino loop.

### III. Fail-Safe Thermostat Safety (NON-NEGOTIABLE)

Thermostat parameters (setpoint, hysteresis, lower/upper limit) MUST pass
`validateThermostatConfig` before being persisted or applied; invalid input is
rejected and logged (`log_w`), never silently clamped or partially applied.
Heater-affecting crons and scheduled state transitions MUST run only when the
clock is synced (`safeCron`, `isClockSynced()`), since acting on an unsynced
clock can turn the heater on/off at the wrong time and for an unknown duration.

**Rationale**: The heater relay directly affects fish health. Any new
feature that reads or writes thermostat config, or that gates heater/CO2/lamp
behavior on time, MUST reuse these existing safety gates rather than
introducing a parallel, unvalidated path.

### IV. Hardware Abstraction & Native Testability

Hardware-facing behavior MUST be exposed through interfaces (`Actuator`,
`TemperatureSensor`, `Clock`, `ThermostatState`) so that business logic — the
thermostat state machine, config validation, clock-sync gating — can be unit
tested on the `env:native` PlatformIO environment with the fakes/stubs under
`test/mocks/` and `test/stubs/`, without a physical ESP32 or its toolchain.
Pure logic (e.g., `validateThermostatConfig`) MUST be written as free
functions so it is testable in isolation from any class. The one documented
exception is a global singleton wrapping an ESP32-only API with no behavior
worth unit-testing (e.g., `TelnetLogger`/`TelnetLog`, `ArduinoClockImpl`/
`ArduinoClock`) — acceptable only for untested platform utilities, never for
logic with behavior worth verifying, which MUST instead go behind an
interface and receive its collaborators via dependency injection (as
`Thermostat` does with `Actuator*`/`Clock*`).

**Rationale**: `env:native` with Unity is the only fast feedback loop this
project has — flashing real hardware for every change is not viable. New
logic that skips the interface boundary is, in practice, untestable and
untested.

### V. Convention Consistency

All C++ code MUST follow `.claude/rules/code-conventions.md` — naming
(PascalCase types, camelCase members, UPPER_SNAKE_CASE macros/pins),
interface naming without an `I` prefix, class section order (`public:` before
`private:`), logging exclusively via ESP-IDF `log_*` macros (never `Serial`,
except the documented `waitWifi` boot-time exception), `constexpr` constants
vs. the `#define` exception for pins/timeouts, and the singleton
declaration/definition pattern for global instances. That file is the single
source of truth for style; this constitution does not duplicate it and
defers to it — a deviation in code is a bug to fix in the code, not a reason
to add a second, competing style rule here.

**Rationale**: A one-person embedded project reviewed across many small
diffs depends on predictable style to stay reviewable; splitting style rules
across two documents invites drift between them.

## Technology Stack & Architecture Boundaries

- **Platform**: Arduino framework on ESP32 (`espressif32`, board `esp32dev`),
  built, flashed, and tested through PlatformIO. Two environments exist:
  `env:esp32dev` (real firmware, OTA/USB upload) and `env:native` (host-side
  unit tests on the `unity` test framework).
- **Layering** (`lib/`): `lib/commons` — platform utilities with no business
  logic (`Clock` interface + `ArduinoClockImpl`/`ArduinoClock` singleton,
  free-function helpers). `lib/devices` — hardware abstractions and the
  thermostat state machine (`Actuator`, `TemperatureSensor`, `Relay`,
  `DallasTemperatureSensor`, `Thermostat`/`ThermostatState`,
  `IdleState`/`HeatingState`, plus the pure `validateThermostatConfig`).
  `lib/mqtt` — `MQTTClient`. `lib/logger` — `TelnetLogger`/`TelnetLog`
  (captures ESP-IDF logs via a linker `--wrap=log_printfv`). `lib/fs` —
  `Config`, persisting OTA password, MQTT settings, cron schedules, and
  thermostat parameters to LittleFS. New hardware or persisted state belongs
  in one of these libs, matched by responsibility — not bolted onto
  `src/main.cpp`.
- **`src/main.cpp` is wiring-only**: it initializes sensors/relays/thermostat,
  brings up WiFi (`WiFiManager` captive portal with custom parameters),
  OTA (`ArduinoOTA`), the HTTP server, MQTT, and cron registrations, and runs
  `setup()`/`loop()`. It MUST NOT accumulate business logic that belongs in a
  `lib/`.
- **Dependencies** are pinned to exact versions in `platformio.ini`
  (`lib_deps`). No dependency is added, removed, or upgraded without explicit
  approval — flash/RAM budget (`min_spiffs` partition) and firmware stability
  depend on a known-good set. `env:native` excludes hardware-bound libs
  (`lib_ignore = commons, fs, mqtt, logger`) and substitutes stub headers
  under `test/stubs/` for `Arduino.h`, `DallasTemperature.h`, `OneWire.h`.
- **Formatting**: Google-based `clang-format` (2-space indent, 80-column
  limit) is authoritative for layout; run it before committing.

## Development Workflow

- **Spec-Driven Development**: every feature beyond a trivial fix is planned
  under `tasks/prd-<feature-name>/` (local, gitignored) as
  `prd.md` → `techspec.md` → `tasks.md` → `codereview.md`, produced by the
  project's `prd-creator` → `techspec-creator` → `task-executor` →
  `task-reviewer` agents in that order. Implementation MUST NOT start ahead
  of an approved PRD and Tech Spec.
- **Testing gate**: any change to logic reachable through an interface listed
  in Principle IV MUST have or update a Unity test under `test/`, and
  `pio test -e native` MUST pass before the change is considered done.
- **Documentation gate**: `README.md`, `CLAUDE.md`, and (if present)
  `AGENTS.md` MUST be updated in the same change set whenever a change
  affects architecture, hardware wiring, dependencies, or conventions already
  documented there.

## Governance

This constitution supersedes ad-hoc practice for architectural decisions;
where it conflicts with a habit not written down anywhere, the constitution
wins. `.claude/rules/code-conventions.md` remains the source of truth for
line-level style (Principle V) and is not superseded by this document.

**Amendments** are made by editing this file directly, updating the Sync
Impact Report at the top, and bumping the version per semantic versioning:

- **MAJOR** — a principle is removed or redefined in a backward-incompatible
  way (e.g., dropping the non-blocking loop requirement).
- **MINOR** — a new principle or materially expanded section is added
  (e.g., a new mandatory testing category).
- **PATCH** — wording, clarification, or typo fixes with no rule change.

Any pull request that touches `src/main.cpp`, `lib/`, or `platformio.ini`
MUST be checked against these principles before merge; a violation MUST
either be fixed or justified in the feature's `techspec.md` with the simpler
alternative explicitly rejected. Complexity or a new dependency without such
justification MUST be rejected in review.

**Version**: 1.0.0 | **Ratified**: 2026-07-05 | **Last Amended**: 2026-07-05
