Strategient-Architect Implementation - Session 7

<title>
Scheduler page: symbol universe + allocator + job plan + runner abstraction (local mock first; SSH/container/k8s later)

<description>
Implement the “scheduled distributed market data retrieval” thin slice. End state: you can define a symbol universe and timeframes, define machines with storage capacity, generate deterministic allocations, and run a mock distributed job that reports progress to the UI. The architecture must support later execution via bare-metal SSH, Docker, or Kubernetes without refactoring the UI/page.

Cursor Prompt to initiate session 7 build (mostly autonomous with some human assistance where needed):
<Prompt>
You are working inside `~/projects/strategient-architect`.

Goal:
- Add `Scheduler` page type for market data retrieval planning/execution.
- Implement a deterministic allocator that assigns symbols across machines by capacity.
- Implement a scheduler loop (timer) and a “Run Now” button.
- Implement a runner abstraction with a local mock runner first.

Hard constraints:
- No assumption of Spark/Airflow/Kafka permanence.
- Execution must be abstracted behind a runner interface that can later target:
  - Local bare metal process
  - SSH to a host
  - Docker container run
  - Kubernetes Job/Pod (via `kubectl` or API)
- v1: do NOT implement real collectors yet; simulate work but keep architecture real.

PlantUML + metadata requirements:
- Scheduler page structure is defined in `page.plantuml` (canonical). Example nodes (with `as` aliases):
  - `UNIVERSE`, `ALLOCATOR`, `RUNNER`, `LOGS`
- Store scheduler settings in metadata, preferably under a known node (e.g. `UNIVERSE`) or under `page.metadata.page_settings`, e.g.:
  - `symbol_universe[]`
  - `timeframes[]`
  - `machines[]` with `machine_id` and `capacity_gb`
  - `schedule_enabled` bool
  - `schedule_interval_sec` int
  - last allocation plan (optional) for inspection/debug

Architecture:
- Add `core/scheduler/IJobRunner` abstract base class:
  - `runOnMachine(machineId, jobs)` returns `{ok, exit_code, stdout, stderr, duration_ms}`
  - emits progress callbacks/signals (or returns structured progress events)
- Implement `LocalMockRunner` that sleeps and emits progress and logs.
- Add `SchedulerService` that:
  - computes allocation plan
  - dispatches to runner for each machine

UI:
- Canvas can show a few nodes: Universe → Allocator → Runner → Logs (simple boxes).
- Inspector allows editing universe/timeframes/capacities and toggling schedule.
- Run Console logs the allocation and run progress.

Acceptance:
- “Run Now” produces deterministic allocations (same input → same output).
- “Schedule Enabled” persists across save/reload.
- Mock runs update progress and logs for at least 2 machines.
</Prompt>

Proposed Implmentation of Session 7:
- Build allocator as pure function with unit tests if you already have a test harness; otherwise verify determinism by repeated runs.
- Runner interface should include an “execution target” descriptor so later you can plug:
  - `SshRunner` (bare metal) using `ssh` process with timeouts.
  - `DockerRunner` using `docker run` process calls.
  - `KubernetesRunner` using `kubectl apply`/`kubectl wait` for Jobs.

Testable Artifacts and Excpected Functions:
- Editable symbol universe and machine capacities.
- “Run Now” generates allocation plan and logs it.
- Mock distributed execution shows progress and finishes.


