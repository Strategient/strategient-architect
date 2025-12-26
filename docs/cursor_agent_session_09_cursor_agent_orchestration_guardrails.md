Strategient-Architect Implementation - Session 9

<title>
Multi-agent orchestration guardrails: ownership boundaries, prompt templates, Cursor CLI integration, and “no haywire” enforcement

<description>
Implement the “agent orchestration without chaos” layer in a minimal but real way. End state: a page/node can have an agent prompt + owned-path manifest; the app can run the Cursor CLI (or a stub command) through a controlled runner, capture outputs, and record an auditable run log (files changed / commands run / tests executed / observed result).

Cursor Prompt to initiate session 9 build (mostly autonomous with some human assistance where needed):
<Prompt>
You are working inside `~/projects/strategient-architect`.

Goal:
- Add a minimal “Agent Run” subsystem that can execute an external command (Cursor CLI or stub) safely and record results.
- Enforce blast-radius limits: an agent run is scoped to a declared set of owned paths.
- Persist agent prompts and run logs in page metadata, keyed by PlantUML node IDs (aliases).

Hard constraints (non-negotiable):
- Orchestrator can only modify files under the page/node’s declared ownership paths.
- Any agent run record must include:
  - files changed (best-effort: git diff file list)
  - commands run
  - tests executed
  - observed result
- Core framework can be set “read-only” (no agent writes) via a setting toggle.
- No embedding “full Cursor IDE” in v1; use CLI invocation and capture stdout/stderr.

Architecture:
- Add `core/agent/IAgentRunner` abstract base class:
  - `run(prompt, workspace_root, ownership_paths, extra_env)` returns `AgentRunResult`
- Implement `ProcessAgentRunner` that runs:
  - either `cursor --version` / `cursor-agent ...` (if available)
  - or a configurable stub command for development
  capturing stdout/stderr + exit code + duration.
- Add `core/agent/OwnershipPolicy`:
  - validates that changed files (from git diff) are within allowed ownership paths.
  - blocks/flags violations and marks run failed.
- Add `core/agent/PromptTemplates` per page type (simple text templates stored in app resources).

IR requirements:
Metadata requirements:
- For each PlantUML node alias (node_id), store:
  - `agent_prompt_text`
  - `ownership_paths[]`
  - `agent_runs[]` with timestamps, command, result summary, and policy outcome.

UI:
- Inspector panel adds an “Agent” section:
  - editable prompt box
  - ownership paths list editor
  - Run button
  - last run status + open log

Acceptance:
- Run a stub agent command and see captured output in UI.
- If the stub command modifies a file outside allowed paths, policy flags it.
- Run records persist after save/reload.
</Prompt>

Proposed Implmentation of Session 9:
- Keep this minimal and auditable; treat it like a “runbook executor” first.
- Add a settings toggle “Framework read-only mode” that disables agent runs by default until you enable it.

Container/Kubernetes note:
- Agent runner should support local bare metal by default.
- Later: allow agent runs inside containers (docker/k8s) by swapping runner implementation without changing the page/node UI.

Testable Artifacts and Excpected Functions:
- Agent run logs are visible, persisted, and scoped by ownership policy.
- Clear “pass/fail” outcome shown in UI and log.


