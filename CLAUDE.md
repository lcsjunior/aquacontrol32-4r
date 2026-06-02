# CLAUDE.md

## Desenvolvimento Orientado a Especificação (SDD)

Este projeto segue **estritamente** um fluxo Spec-Driven Development conduzido pelos
agentes em `.claude/agents/`. Os artefatos de cada feature vivem em
`tasks/prd-<nome-da-feature>/`.

## Visão Geral

Firmware ESP32 para um controlador de aquário plantado, em placa LILYGO T-Relay.
Hardware (pinout) e mapeamento de fields do ThingSpeak: ver `README.md`.

## Arquitetura (onde as coisas vivem)

- `src/main.cpp` — wiring geral: inicializa sensores, relés e thermostat; conecta
  WiFi+MQTT; registra crons e o servidor HTTP; roda o loop principal.
- `lib/commons/` — utilitários de plataforma: `espx_wifi` (STA+AP, OTA, NTP),
  `clock.h` (interface `Clock`), `arduino_clock` (impl concreta).
- `lib/devices/` — abstrações de hardware: `Relay` (atuador GPIO), `DallasTemperatureSensor`
  (DS18B20), `Thermostat` + `ThermostatState` (máquina de estados: `IdleState`,
  `HeatingState`), interfaces `Actuator` e `TemperatureSensor`.
- `lib/mqtt/` — `MQTTClient` (publicação de dados ao broker MQTT).
- `lib/config/` — `Config` (setpoint, hysteresis) persistido em LittleFS.

Detalhes que mudam (horários dos crons, endpoints HTTP, fields MQTT, versões de libs)
vivem no código (`src/main.cpp`) e no `platformio.ini` — não duplicar aqui.
