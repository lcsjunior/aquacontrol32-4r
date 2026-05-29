# CLAUDE.md

## Desenvolvimento Orientado a Especificação (SDD)

Este projeto segue **estritamente** um fluxo Spec-Driven Development conduzido pelos
agentes em `.claude/agents/`. Os artefatos de cada feature vivem em
`tasks/prd-<nome-da-feature>/`.

> **REGRA CRÍTICA:** NUNCA implemente uma nova feature ou mudança de comportamento
> direto no código. SEMPRE roteie o trabalho pelos agentes do SDD. Conclua **uma
> etapa por vez** e **pare**, aguardando aprovação antes de avançar — mesmo que o
> usuário acione um único agente.

**Pipeline (ordem obrigatória):** `prd-creator` → `techspec-creator` →
`task-creator` → `task-executor` → `task-reviewer`.

O que cada agente produz e quando acioná-lo está no frontmatter em `.claude/agents/`.

## Skills

Ficam em `.claude/skills/` e são selecionadas automaticamente pela descrição.
Consulte o `SKILL.md` correspondente antes de implementar ou revisar:

- `embedded-cpp` — firmware C++ (memória, ISR, flash strings, relay/sensor).
- `platformio-build` — build, flash, monitor, `platformio.ini`.
- `cpp-coding-standards` — escrever/revisar/refatorar C++.

## Visão Geral

Firmware ESP32 para um controlador de aquário plantado, em placa LILYGO T-Relay.
Hardware (pinout), mapeamento de fields do ThingSpeak e fotos do build: ver `README.md`.

## Arquitetura (onde as coisas vivem)

- `src/main.cpp` — wiring geral: inicializa sensores, relés e thermostat; conecta
  WiFi+MQTT; registra crons e o servidor HTTP; roda o loop principal.
- `lib/commons/` — abstrações de hardware: `relay`, `temp_sensor` (DS18B20),
  `thermostat` (máquina de estados), `espx_wifi` (STA+AP, OTA, NTP, utilidades).
- `lib/config/` — `Config` (setpoint, hysteresis) persistido em LittleFS.

Detalhes que mudam (horários dos crons, endpoints HTTP, fields MQTT, versões de libs)
vivem no código (`src/main.cpp`) e no `platformio.ini` — não duplicar aqui.
