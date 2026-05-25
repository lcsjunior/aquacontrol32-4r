# Tarefa 1.0: Remover field2 de writeMsg() e verificar build

## Visão geral

Editar a função `writeMsg()` em `src/main.cpp` removendo o especificador de formato `&field2=%.1f` da string PROGMEM e o argumento `tempSensor2.getCTemp()` da chamada `snprintf_P`. Em seguida, confirmar que `pio run` compila sem erros nem novos warnings.

<skills>
### Skills aplicáveis

- `embedded-cpp`: manter uso de `PSTR()` + `snprintf_P`; preservar buffer fixo `msg[255]`; não introduzir `String` nem `malloc`; garantir alinhamento correto entre especificadores de formato e argumentos variádicos.
- `platformio-build`: executar `pio run` ao final para confirmar build limpa; nenhuma alteração em `platformio.ini`.
</skills>

<requirements>

- RF-01: A string de formato PSTR em `writeMsg()` não deve conter `field2`.
- RF-02: O argumento `tempSensor2.getCTemp()` deve ser removido da lista de argumentos de `snprintf_P`.
- RF-03: `pio run` deve compilar sem erros nem novos warnings.
</requirements>

## Subtarefas

- [ ] 1.1 Em `src/main.cpp`, remover `&field2=%.1f` da string de formato PROGMEM dentro de `writeMsg()`.
- [ ] 1.2 Remover o argumento `tempSensor2.getCTemp()` da lista de argumentos variádicos de `snprintf_P`, mantendo todos os demais argumentos na ordem correta.
- [ ] 1.3 Executar `pio run` e confirmar compilação sem erros.

## Critérios de sucesso

- `pio run` compila sem erros.
- A string `msg` gerada por `writeMsg()` não contém `field2=`.
- Os campos `field1`, `field3`, `field4`, `field5`, `field6` e `status` continuam presentes e com os argumentos corretos (verificável via `GET /msg` ou monitor serial).

## Arquivos relevantes

- `/home/lc/Projects/aquacontrol32-4r/src/main.cpp` — função `writeMsg()`, linhas 120–130.
