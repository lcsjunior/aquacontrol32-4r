# Convenções do Projeto

## Idioma

- Código, identificadores, comentários e strings de log: **inglês**.

## Nomenclatura

Segue a convenção Arduino, que sobrescreve a nomenclatura recomendada nas skills:

- `PascalCase` — classes/tipos (ex.: `Relay`, `Thermostat`, `DSTempSensor`).
- `camelCase` — métodos, funções, variáveis e atributos (ex.: `turnOn`, `getCTemp`).
- `UPPER_SNAKE_CASE` — macros e definições de pino (ex.: `RELAY_PIN_1`, `DS_PIN_1`).

Nos demais casos, seguir as convenções recomendadas nas skills.

## Dependências

- Bibliotecas ficam fixadas por versão em `platformio.ini`.
- Não adicionar, remover ou atualizar dependências sem aprovação explícita.

## Hardware como fonte de verdade

Quando um guideline de C++ conflitar com uma restrição de hardware (pinagem,
timing, memória do ESP32), a restrição de hardware prevalece.
