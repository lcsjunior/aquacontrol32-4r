---
name: platformio-build
description: Workflow de build, flash e configuração do PlatformIO para ESP32 — pio run (compilação), secrets.h obrigatório, dependências em platformio.ini, upload/monitor requerem USB físico.
---

# Skill: platformio-build

Workflow de build, flash e configuração do PlatformIO para este projeto ESP32.

## Comandos Principais

```bash
# Compilar (funciona em sessão remota)
pio run

# Upload para o dispositivo (requer USB físico — não funciona em sessão remota)
pio run --target upload

# Monitor serial a 115200 baud (requer USB físico — não funciona em sessão remota)
pio device monitor

# Build + upload + monitor de uma vez
pio run --target upload && pio device monitor
```

## secrets.h

`src/secrets.h` está no `.gitignore` e deve existir localmente antes do build. Verifique a presença de todos os defines:

- `WIFI_SSID`, `WIFI_PASS`
- `OTA_PASS`, `AP_PASS`
- `MQTT_CLIENT_ID`, `MQTT_USERNAME`, `MQTT_PASSWORD`
- `SMART_AQUARIUM_WR_CH_ID`

**Nunca exiba os valores** — apenas confirme a presença de cada `#define`. Se o arquivo estiver ausente, o build falha com erro de include.

## Dependências

Gerenciadas em `platformio.ini`. Não altere versões de bibliotecas sem verificar breaking changes no changelog de cada lib.

| Biblioteca         | Versão  | Uso                        |
|--------------------|---------|----------------------------|
| ArduinoJson        | 6.21.2  | Serialização de config      |
| DallasTemperature  | 3.11.0  | Sensor DS18B20              |
| OneWire            | 2.3.7   | Protocolo OneWire           |
| CronAlarms         | 0.1.0   | Agendamento cron-style      |
| NoDelay            | 2.1.1   | Timers não-bloqueantes      |
| PubSubClient       | 2.8     | Cliente MQTT                |

**Diretriz para sessão local sem dispositivo:** Se compilação passa (`pio run` OK), o código está correto e pronto. Relate sucesso da build e indique que upload/monitor requerem ESP32 conectado via USB.
