Strategient-Architect Implementation - Session 3

<title>
PlantUML-canonical project model + Document/Page OO spine + `.sarch.json` load/save + Pages sidebar (switching works)

<description>
Implement the project file format and strict OO interfaces so new pages cannot be “half-implemented”. End state: you can load a sample `.sarch.json` containing multiple pages, switch pages from a sidebar, and save/reload with exact round-trip of PlantUML text + per-node metadata.

Cursor Prompt to initiate session 3 build (mostly autonomous with some human assistance where needed):
<Prompt>
You are working inside the existing repo `~/projects/strategient-architect`.

Goal:
- Implement the canonical project model (JSON) and a `DocumentModel` owned by the Qt app.
- Add a Pages sidebar that lists pages and allows switching the active page.
- Persistence must round-trip cleanly: load → modify → save → reload preserves all fields.

Hard constraints:
- PlantUML is canonical for diagram structure and boundaries. The user pastes PlantUML DSL into the app.
- The app derives a structured “project model” from PlantUML so it can be clickable/executable, but it must not silently rewrite the user’s PlantUML.
- All per-node/per-edge extra data (prompts, code/config pointers, status, layout) is stored as metadata keyed by stable PlantUML IDs (aliases).
- No Python in core framework.
- Keep builds clean on Ubuntu.

Project file requirements (v1 scope):
- Project file extension: `.sarch.json`
- Top-level:
  - `schema_version` (int, start at 1)
  - `project_id` (string)
  - `title` (string)
  - `pages` (array)
- Each page:
  - `page_id`, `title`
  - `page_type` (string enum; start with: `PipelineDiagram`, `InfraConfig`, `Scheduler`, `BacktestSweeps`, `DiagramOnly`)
  - `plantuml` (string)  <-- canonical diagram definition
  - `metadata` (object)  <-- extra data keyed by PlantUML IDs
    - `nodes` (object keyed by node_id/alias)
    - `edges` (object keyed by edge_id if available; otherwise by `from->to` key)
    - `layout` (object keyed by node_id: x/y)
    - `runs` / `status` / `tasks` (optional)

PlantUML ID rule (required for clickability):
- Every clickable node in PlantUML MUST have a stable alias using `as`, e.g.:
  - `component "Spark Master" as SPARK_MASTER`
  - `rectangle "Universe" as UNIVERSE`

Metadata requirements (v1 minimal):
- For each node_id key (from PlantUML alias), metadata stores:
  - `display_name` (optional override)
  - `agent_prompt_text` (string)
  - `ownership_paths[]` (optional, used later for agent guardrails)
  - `code_refs[]` (optional: file path(s) or command(s) that implement the node)
  - `test_status` enum: `not_run|pass|fail`
  - `tasks[]` with `{id,title,status}`

Qt requirements:
- `DocumentModel` owns the project model, exposes current page, and emits signals on change.
- UI adds a Pages sidebar (dock or left panel) listing pages; clicking switches active page.
- Central canvas can remain placeholder, but MUST update a visible label like “Active Page: <title>” for now.
- Add File menu: Open Project, Save Project As (or Save).
- Include `assets/sample_project.sarch.json` containing at least 2 pages, each with valid PlantUML text with `as` aliases.

Acceptance checks:
- Open `assets/sample_project.sarch.json` via UI.
- Switch pages and observe active page title changes.
- Save to a new file, then reopen; confirm fields remain (PlantUML text + node metadata).
- Project JSON is human-readable and stable (pretty-printed is fine).
</Prompt>

Proposed Implmentation of Session 3:
- Introduce `architect::project::*` structs (or classes) with `toJson()/fromJson()` methods.
- Add `DocumentModel` (QObject) with:
  - `loadFromFile(path)`, `saveToFile(path)`
  - `pages()`, `currentPageId`, `setCurrentPage(id)`
  - Signals: `documentLoaded`, `currentPageChanged`, `dirtyChanged`
- Add UI:
  - Pages list view (QListWidget is fine for v1).
  - Update main window title or a visible label to show active page.

Design notes (important):
- We keep PlantUML canonical, but we do NOT attempt perfect round-trip structural edits in v1.
- v1 workflow:
  - user edits PlantUML for structure
  - user edits metadata in the app for execution/prompt/status/layout
  - app saves both; parsing happens on load/render

Testable Artifacts and Excpected Functions:
- Open/save `.sarch.json` works.
- Page switching works and is visible in UI.
- Round-trip of sample project preserves PlantUML and metadata.


