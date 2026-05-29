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
- `lib/commons/` — abstrações de hardware: `relay`, `temp_sensor` (DS18B20),
  `thermostat` (máquina de estados), `espx_wifi` (STA+AP, OTA, NTP, utilidades).
- `lib/config/` — `Config` (setpoint, hysteresis) persistido em LittleFS.

Detalhes que mudam (horários dos crons, endpoints HTTP, fields MQTT, versões de libs)
vivem no código (`src/main.cpp`) e no `platformio.ini` — não duplicar aqui.
