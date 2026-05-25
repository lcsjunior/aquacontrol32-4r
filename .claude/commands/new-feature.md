Inicie o fluxo SDD completo para uma nova feature ou correção neste projeto de firmware ESP32.

**Fase 1 — Planejamento (@sdd-planner):**

1. Invoque `@sdd-planner` — ele conduzirá as três fases de planejamento em sequência:
   - **PRD**: esclarecimento → exploração → documento de requisitos (`tasks/prd-[slug]/prd.md`)
   - **TechSpec**: análise técnica → especificação de arquitetura (`tasks/prd-[slug]/techspec.md`)
   - **Tasks**: lista de entregas + arquivos individuais (`tasks/prd-[slug]/tasks.md` e `[N]_task.md`)
2. Aguarde aprovação do usuário em cada fase antes de avançar

**Fase 2 — Implementação (@sdd-implementer):**

3. Invoque `@sdd-implementer` com o slug da feature
4. O implementer lerá cada `[N]_task.md`, implementará e rodará `pio run` após cada tarefa
5. Tarefas concluídas são marcadas em `tasks.md`

**Fase 3 — Revisão (@sdd-reviewer):**

6. Invoque `@sdd-reviewer` com o slug da feature
7. O reviewer verificará o diff, rodará `pio run` e aplicará o checklist embedded-cpp
8. Aguarde o resultado (APROVADO / APROVADO COM RESSALVAS / REPROVADO)
