# Convenções do Projeto

## Idioma

- Código, identificadores, comentários e strings de log: **inglês**.

## Nomenclatura

Segue a convenção Arduino, que sobrescreve a nomenclatura recomendada nas skills:

- `PascalCase` — classes/tipos (ex.: `Relay`, `Thermostat`, `DSTempSensor`).
- `camelCase` — métodos, funções, variáveis e atributos (ex.: `turnOn`, `getCTemp`).
- `UPPER_SNAKE_CASE` — macros e definições de pino (ex.: `K1_PIN`, `ONEWIRE_PIN_1`).

### Singletons globais

Seguindo o estilo das libs do core Arduino (`Serial`, `Wire`, `SPI`, `EEPROM`), instâncias
globais únicas usam **PascalCase** (não camelCase). Quando o nome desejado para a instância
colidir com o nome da classe concreta, sufixar a classe com `Impl`:

- Classe concreta: `class ArduinoClockImpl : public Clock { ... };`.
- Instância: `extern ArduinoClockImpl ArduinoClock;` (PascalCase, nome curto e distintivo).

Nos demais casos, seguir as convenções recomendadas nas skills.

## Dependências

- Bibliotecas ficam fixadas por versão em `platformio.ini`.
- Não adicionar, remover ou atualizar dependências sem aprovação explícita.

## Comentários de código

- Não adicionar comentários ao código. Identificadores bem nomeados são suficientes.
- Nomes de métodos, funções e variáveis devem ser **sugestivos**: revelar intenção e comportamento sem precisar de explicação adicional.

## Hardware como fonte de verdade

Quando um guideline de C++ conflitar com uma restrição de hardware (pinagem,
timing, memória do ESP32), a restrição de hardware prevalece.
