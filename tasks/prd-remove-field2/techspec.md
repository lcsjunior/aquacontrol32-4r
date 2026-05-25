# TechSpec — Remover field2 da Publicação MQTT ao ThingSpeak

## Resumo executivo

A mudança é cirúrgica e confinada à função `writeMsg()` em `src/main.cpp`. Consiste em remover o especificador `&field2=%.1f` da string de formato PROGMEM e o argumento `tempSensor2.getCTemp()` da chamada `snprintf_P`. Nenhuma outra função, classe ou arquivo é afetado.

## Arquitetura

### Componentes afetados

- `src/main.cpp` — única alteração: corpo da função `writeMsg()` (linha 123–129).

### Principais interfaces

Assinatura atual de `writeMsg()` (sem mudança de assinatura — apenas implementação interna):

```cpp
void writeMsg();
```

Chamada atual a `snprintf_P` (a ser modificada):

```cpp
snprintf_P(
    msg, sizeof(msg),
    PSTR("field1=%.1f&field2=%.1f&field3=%d&field4=%d&field5=%d&field6=%d&"
         "status=PUB %s RSSI %d dBm (%d pcent)"),
    tempSensor1.getCTemp(), tempSensor2.getCTemp(), heater1.isOn(),
    heater2.isOn(), lamp.isOn(), co2Valve.isOn(), tbuf, WiFi.RSSI(),
    dBm2Quality(WiFi.RSSI()));
```

Resultado esperado após a mudança:

```cpp
snprintf_P(
    msg, sizeof(msg),
    PSTR("field1=%.1f&field3=%d&field4=%d&field5=%d&field6=%d&"
         "status=PUB %s RSSI %d dBm (%d pcent)"),
    tempSensor1.getCTemp(), heater1.isOn(),
    heater2.isOn(), lamp.isOn(), co2Valve.isOn(), tbuf, WiFi.RSSI(),
    dBm2Quality(WiFi.RSSI()));
```

### Modelos de dados

Nenhuma struct, enum ou constante nova ou modificada. O buffer `msg[255]` permanece inalterado — a string resultante ficará aproximadamente 12 bytes menor, dentro da capacidade existente.

## Sequenciamento de implementação

1. Editar `writeMsg()` em `src/main.cpp` — remoção do especificador e do argumento (alteração única, sem dependências).
2. Executar `pio run` — confirmar compilação limpa.

## Verificação

- `pio run` compila sem erros nem warnings novos.
- Via `GET /msg` ou monitor serial: a resposta não deve conter a substring `field2=` na carga útil.
- Os campos `field1`, `field3`, `field4`, `field5`, `field6` e `status` devem continuar presentes e com valores corretos.

## Conformidade com skills

- `embedded-cpp`: formato PROGMEM (`PSTR()` + `snprintf_P`) preservado; buffer fixo `msg[255]` mantido; nenhum uso de `String` ou `malloc`.
- `platformio-build`: nenhuma alteração em `platformio.ini` ou dependências necessária.

## Riscos conhecidos

- Desalinhamento de argumentos variádicos: ao remover `&field2=%.1f` da format string é obrigatório remover simultaneamente `tempSensor2.getCTemp()` da lista de argumentos — caso contrário todos os campos numéricos subsequentes receberão valores incorretos. Este é o único ponto crítico da mudança.

## Arquivos relevantes

- `/home/lc/Projects/aquacontrol32-4r/src/main.cpp` — único arquivo a modificar.
