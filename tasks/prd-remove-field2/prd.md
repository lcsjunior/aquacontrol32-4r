# PRD — Remover field2 da Publicação MQTT ao ThingSpeak

## Visão geral

Atualmente, a mensagem publicada ao ThingSpeak via MQTT inclui o parâmetro `field2=%.1f`, que representa a temperatura do segundo sensor DS18B20 (`tempSensor2`). O usuário solicita a remoção desse campo da carga útil enviada. O canal ThingSpeak continuará recebendo os demais campos sem interrupção.

## Objetivos

- A mensagem MQTT publicada a cada 20 segundos não deve conter o parâmetro `field2`.
- Os campos restantes (`field1`, `field3`, `field4`, `field5`, `field6`) devem continuar sendo enviados com os mesmos valores e na mesma ordem lógica.
- O argumento correspondente a `tempSensor2.getCTemp()` na chamada a `snprintf_P` deve ser removido junto com o especificador de formato, evitando desalinhamento de argumentos.
- O sensor `tempSensor2` e o `thermostat2` continuam operacionais — a remoção é restrita ao payload MQTT.

## Requisitos funcionais

1. RF-01: A função `writeMsg()` em `src/main.cpp` não deve incluir `field2` na string de formato PSTR passada a `snprintf_P`.
2. RF-02: O argumento `tempSensor2.getCTemp()` deve ser removido da lista de argumentos variádicos de `snprintf_P` em `writeMsg()`.
3. RF-03: A compilação deve passar sem erros nem novos warnings (`pio run`).

## Restrições de hardware

- O sensor DS18B20 no pino `DS_PIN_2` (`DS_PIN_2 = 23`) e o `thermostat2` continuam inicializados e operando normalmente — apenas a publicação do valor no MQTT é suprimida.
- O buffer `msg[255]` deve permanecer com o mesmo tamanho; a string resultante será menor, sem necessidade de redimensionamento.

## Fora do escopo

- Remoção do sensor `tempSensor2`, da instância `thermostat2` ou de qualquer lógica de controle de temperatura.
- Alteração de outros campos MQTT (`field1`, `field3`–`field6` e `status`).
- Mudanças na configuração do canal ThingSpeak.
- Alterações em `platformio.ini` ou em bibliotecas.
