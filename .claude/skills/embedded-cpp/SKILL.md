---
name: embedded-cpp
description: Regras de C++ para firmware embarcado ESP32 — sem malloc fora de construtores globais, strings flash com F()/PSTR(), ISR/callbacks sem bloqueios, buffers fixos, sensores via abstrações.
---

# Skill: embedded-cpp

Regras de C++ para firmware embarcado ESP32. Consulte antes de implementar ou revisar qualquer arquivo em `src/` ou `lib/`.

## Memória

- Sem `malloc()` nem `new` fora de construtores de objetos globais — heap fragmentation é fatal em embedded
- Use buffers fixos: `char topic[32]`, `char msg[255]` (ver main.cpp como referência)
- Nunca use a classe `String` do Arduino — use `char[]` + `snprintf_P`

## Strings Flash

- `F()` em `Serial.print()` — mantém literais na flash, não na RAM
- `PSTR()` + `snprintf_P` para format strings com argumentos (ver `writeMsg()` em main.cpp)
- Prefira `FPSTR()` ao passar strings PROGMEM para funções que aceitam `const __FlashStringHelper*`

## ISR / Callbacks

- Callbacks de Cron: APENAS `relay.turnOn()` / `relay.turnOff()` — sem `delay()`, sem `Serial`, sem leitura de sensor
- Nunca bloqueie em contexto de interrupção ou callback de timer
- Não chame `Wire`, `SPI` ou `Serial` fora do loop principal

## Sensores

- DS18B20 sempre via `DSTempSensor::getCTemp()` — nunca acesse `DallasTemperature` diretamente
- `requestTemperatures()` é throttled internamente na classe — não chame externamente

## Relay

- Relays acionados apenas por `Thermostat::handleHeater()` ou handlers HTTP registrados em `initWS()`
- Chamada direta de `relay.turnOn/Off()` fora desses pontos requer comentário justificando o motivo

## Timers Não-Bloqueantes

- Use `noDelay` para timeouts (já presente: `mqttConnTime`, `mqttPubTime`)
- Nunca use `delay()` no `loop()` — paralisa Wifi, OTA e MQTT
