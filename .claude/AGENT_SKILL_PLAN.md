# Agent Skill Plan

## Task

scan DevMeoU README-evaluated AI repos, inspect provided skills MCP prompts token routing, install token-saving default skill plan for Scrum Master, Supervisor, code-review-graph, model optimizer, and ESP-IDF firmware

## Installed / Recommended Skills

- `esp-idf-firmware` - ESP-IDF/MCU firmware signals detected.
  - Source: D:\Workspace\Project\Agents-common-installer\skills\esp-idf-firmware
- `supervisor-agents` - Task asks for supervision, review, ADR, architecture, plan, or diff compliance.
  - Source: D:\Workspace\Project\Agents-common-installer\skills\supervisor-agents
- `scrum-master` - Task asks for Scrum, sprint, backlog, retro, velocity, or story workflow.
  - Source: D:\Workspace\Project\Agents-common-installer\skills\scrum-master
- `project-skill-recommender` - Task asks to evaluate or install project skills.
  - Source: D:\Workspace\Project\Agents-common-installer\skills\project-skill-recommender
- `senior-pm` - Baseline common project skill.
  - Source: D:\Workspace\Project\Agents-common-installer\skills\senior-pm
- `model-task-optimizer` - Baseline common project skill.
  - Source: D:\Workspace\Project\Agents-common-installer\skills\model-task-optimizer

## Token-Saving Rules

- Read only the one most specific `SKILL.md` before a task; do not bulk-read every skill.
- Use baseline skills as routing rules: project-skill-recommender for onboarding, supervisor-agents for review/compliance, scrum-master for sprint/backlog, senior-pm for roadmap/risk/status, model-task-optimizer for prompt/model/token routing.
- Prefer code-review-graph for repository context before large diff reviews instead of dumping whole files into context.
- Keep project evidence citations short: file path + purpose + exact command output where possible.
- Use subagents/passes only when the work benefits from role separation; otherwise run sequential lightweight passes.

## Common Project Prompts

### Scrum Master / Alignment Prompt

Use `.claude/commands/scrum-align.md`. Inspect README, docs, ADRs, roadmap/backlog, git diff, code graph, and tests. Produce an alignment matrix and next sprint items.

### Supervisor / Code Review Prompt

Use `supervisor-agents` when reviewing changes. Run Diff Analyzer, Context Checker, Plan Validator, ADR Auditor, and Aggregator roles. Mark missing evidence explicitly.

### Code Graph Prompt

Use MCP `code-review-graph` when available. Build/update graph, detect changed impact, identify hubs/flows/tests, then review only impacted context first.

### Model / Token Optimizer Prompt

Use `model-task-optimizer` for large tasks. Recommend primary/fallback models, context strategy, batching, prompt caching, validation, and token budget before execution.

## Execution Flow

1. Confirm project root and git state.
2. Read this plan and the single most relevant skill.
3. Build/update code-review-graph when useful.
4. Create a small task plan with acceptance checks.
5. Execute with focused agents/passes.
6. Run project-specific tests/build/lint.
7. Re-run supervisor alignment for any diff.
8. Report: changed files, commands, evidence, remaining blockers, next actions.
