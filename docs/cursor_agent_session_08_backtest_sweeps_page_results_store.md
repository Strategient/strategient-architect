Strategient-Architect Implementation - Session 8

<title>
Backtest sweeps page: local parallel runner + Rust engine stub + results store (DuckDB/SQLite) + metrics table UI

<description>
Implement the “Spark-based concurrent Rust backtest sweeps / strategy factory” thin slice without requiring a Spark cluster yet. End state: clicking “Run Sweeps” executes N simulated backtests concurrently (local worker pool), streams progress into UI, and persists results to a local results database file. The architecture must keep the door open to Spark/containers/K8s later (plan generator + runner abstraction).

Cursor Prompt to initiate session 8 build (mostly autonomous with some human assistance where needed):
<Prompt>
You are working inside `~/projects/strategient-architect`.

Goal:
- Add `BacktestSweeps` page type.
- Implement a local parallel runner (QtConcurrent or std::thread pool).
- Extend Rust engine with a stub `run_backtest(symbol, params_json)` that sleeps and returns fake metrics JSON.
- Persist results to DuckDB (default for v1; used for in-process analytics).
- Display results in a sortable Qt table view.

Hard constraints:
- Do NOT require Spark yet.
- Keep architecture compatible with later distributed execution:
  - Generate a “plan” JSON that *could* map to spark-submit or k8s jobs later.
  - Execute the plan with a runner interface; v1 runner is local.

PlantUML + metadata requirements:
- BacktestSweeps page structure is defined in `page.plantuml` (canonical) with aliases for:
  - `UNIVERSE`, `PARAM_GRID`, `RUNNER`, `RESULTS_STORE`, `METRICS_VIEW`
- Persist sweep settings in metadata (e.g. `page.metadata.page_settings`):
  - universe list
  - param grid spec (simple JSON)
  - results db path
  - last run summary (optional)

UI requirements:
- Nodes represent: Universe → ParamGrid → Runner → ResultsStore → MetricsView
- Status badges update while running.
- Table view shows at least: symbol, params hash, pnl, sharpe, max_dd, duration_ms.

Acceptance:
- Clicking “Run Sweeps” runs 20 simulated jobs concurrently (configurable).
- UI shows progress and final summary counts.
- Results persist to the DB file; re-opening the project can reload and display the last results.
</Prompt>

Proposed Implmentation of Session 8:
- Use DuckDB for v1 (in-process analytics). Keep the storage layer behind a small interface so swapping to SQLite/Postgres later is low-friction.
- Keep the engine boundary clean:
  - C ABI function returns metrics as JSON string (or a simple struct) + free function.
- Plan generator outputs:
  - list of (symbol, params_json, run_id)
  - execution settings (max_concurrency)

Container/Kubernetes note:
- Runner interface should be able to switch to:
  - Spark submit runner (bare metal or container)
  - Kubernetes Jobs runner (container)
  - SSH runner (bare metal)
  without changing page UI/IR fields.

Testable Artifacts and Excpected Functions:
- Run sweeps locally and see table fill.
- Close/reopen app and reload stored results from DB.


