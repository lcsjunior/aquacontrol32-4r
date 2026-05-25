Start the full SDD flow for a new feature or fix in this ESP32 firmware project.

**Phase 1 — Planning (@sdd-planner):**

1. Invoke `@sdd-planner` — it will conduct all three planning phases in sequence:
   - **PRD**: clarification → exploration → requirements document (`tasks/prd-[slug]/prd.md`)
   - **TechSpec**: technical analysis → architecture spec (`tasks/prd-[slug]/techspec.md`)
   - **Tasks**: list of deliverables + individual files (`tasks/prd-[slug]/tasks.md` and `[N]_task.md`)
2. Wait for user approval at each phase before advancing

**Phase 2 — Implementation (@sdd-implementer):**

3. Invoke `@sdd-implementer` with the feature slug
4. The implementer will read each `[N]_task.md`, implement, and run `pio run` after each task
5. Completed tasks are marked in `tasks.md`

**Phase 3 — Review (@sdd-reviewer):**

6. Invoke `@sdd-reviewer` with the feature slug
7. The reviewer will check the diff, run `pio run`, and apply the embedded-cpp checklist
8. Wait for the result (APPROVED / APPROVED WITH CAVEATS / REJECTED)
