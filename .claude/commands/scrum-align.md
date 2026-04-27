# Scrum Align

Goal: make the current implementation match the repository design/plan/ADR/docs, with supervision and role-based execution.

Workflow:

1. Bootstrap context
   - Read project-local skills in `.claude/skills` when relevant.
   - Inspect `README.md`, `AGENTS.md`, `CLAUDE.md`, `docs/`, `architecture/`, `docs/adr/`, `adr/`, `PLAN.md`, `TODO.md`, and roadmap files when present.
   - Run `git status --short`, `git diff --stat`, and `git diff --find-renames`.

2. Build/update code graph
   - Prefer MCP `code-review-graph` tools if available.
   - Otherwise run:
     - `uvx code-review-graph status`
     - `uvx code-review-graph build` if missing/stale
     - `uvx code-review-graph detect-changes` for current diff impact
   - Use graph results to identify impacted files, callers, tests, flows, hubs, and bridge nodes.

3. Scrum/design alignment check
   - Treat design docs, ADRs, implementation plan, acceptance criteria, and tests as source of truth.
   - Produce a concise alignment matrix:
     - Requirement/design item
     - Evidence in code/tests/docs
     - Status: compliant / partial / missing / violation / unknown
     - Owner role
     - Next action

4. Supervisor review
   - Use the `supervisor-agents` skill rules.
   - Run independent passes/agents when available:
     - Diff Analyzer
     - Context Checker
     - Plan Validator
     - ADR Auditor
     - Aggregator
   - Do not hide uncertainty; mark missing evidence explicitly.

5. Role-based execution loop
   - Create or simulate these roles as subagents/passes:
     - Dev Agent: implement missing/incorrect behavior.
     - Test Agent: add/update unit/integration/e2e tests.
     - QA Agent: verify acceptance criteria and user flows.
     - QC Agent: check quality, lint, formatting, regressions, and edge cases.
     - Docs Agent: update docs/ADR/plan when implementation intentionally changes design.
     - Supervisor Agent: re-review every loop against design/ADR/plan/diff.
   - If subagent tools are available, spawn agents with focused tasks; otherwise run the passes sequentially.

6. Iterate until done
   - After each implementation pass, run relevant tests/lint/build.
   - Re-run code-review-graph update/detect-changes.
   - Re-run supervisor alignment.
   - Continue until all blocker/high/required alignment issues are resolved or a human decision is required.

7. Stop conditions
   - Stop and ask the human when requirements conflict, destructive/external actions are needed, credentials are required, or a design decision is ambiguous.
   - Otherwise keep working until the project is compliant.

Final report:

- Overall status: compliant / mostly compliant / mixed / blocked
- Commands run
- Files changed
- Tests/build/lint result
- Alignment matrix summary
- Remaining risks/blockers
- Suggested next sprint/backlog items
