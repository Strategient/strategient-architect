Strategient-Architect Implementation - Session 5

<title>
PlantUML workflow (canonical): paste/edit PlantUML, validate/render, parse into clickable UI diagram, and attach per-node metadata (no auto-download)

<description>
Add the PlantUML service/pipeline that makes your core workflow real: PlantUML text is canonical, it’s pasted/edited inside the app, then the app validates/renders it and converts it into a clickable UI diagram. End state: you can paste PlantUML, click Render, see the diagram, click a block, and edit that block’s prompt/config/status in the inspector.

Cursor Prompt to initiate session 5 build (mostly autonomous with some human assistance where needed):
<Prompt>
You are working inside `~/projects/strategient-architect`.

Goal:
- Add PlantUML rendering capability (text → SVG) using a local `plantuml.jar`.
- Add UI to view/preview rendered SVG in-app.
- Treat PlantUML as canonical for diagram structure/boundaries, and store extra per-node metadata separately (keyed by stable IDs).

Hard constraints:
- Do NOT download anything at runtime (no jar fetch).
- Rendering must be explicit (button click), not on every keystroke.
- If PlantUML text is large, reject (limit ~256KB).
- Rendering must fail gracefully and show error output.

Data model:
- All diagram page types (`InfraConfig`, `Scheduler`, `BacktestSweeps`, etc.) include:
  - `page.plantuml` (string)  <-- canonical
  - `page.metadata.nodes[node_id]` (object)  <-- prompts/config/status keyed by PlantUML alias

Stable ID rule (required):
- Every clickable block must have a PlantUML alias using `as`, e.g.:
  - `component "Airflow" as AIRFLOW`
  - `rectangle "Run Sweeps" as RUN_SWEEPS`

Implementation requirements:
- Add `services/PlantUmlService` that runs:
  - `java -jar <jarPath> -tsvg <input.puml>`
  - Output SVG goes to app cache dir: `~/.cache/strategient-architect/plantuml/`
  - Capture stdout/stderr and exit codes.
- Add a minimal `services/PlantUmlParser` (or `core/plantuml/PlantUmlParser`) that parses a supported subset:
  - node declarations with `as ALIAS`
  - relationships like `A --> B : label`
  - (v1) ignore advanced layout; use metadata.layout for positions
- Add `ui/inspector/PlantUmlInspectorPanel`:
  - QPlainTextEdit (monospace) for PlantUML text
  - Buttons: Validate, Render, Open SVG (external), Copy SVG path
  - Status line with success/error summary
- Add `ui/panels/PlantUmlPreviewPanel` that displays an SVG via `QGraphicsSvgItem` or `QSvgWidget`.
- Wire it up:
  - For any page type that has `page.plantuml`, inspector shows PlantUML editor.
  - Render writes back to `page.plantuml`, renders SVG, parses to nodes/edges, and updates the clickable canvas view.
  - Clicking a node opens/edit metadata for that node_id (prompt/config/status).

Acceptance:
- Open a page from the sample project.
- Edit PlantUML text, click Render, see SVG update and clickable nodes appear.
- Click a node, edit its `agent_prompt_text` in metadata, save/reload and verify it persists.
</Prompt>

Proposed Implmentation of Session 5:
- Implement PlantUML rendering service and cache strategy.
- Implement the PlantUML editor + render + parse loop that turns PlantUML into a clickable diagram UI.
- Keep dependencies minimal: QtSvg; QtWebEngine is not required.

Container/Kubernetes note:
- Jar path should be configurable (env var or settings) so it works on bare metal or inside containers.
- Do NOT assume any particular deployment; just locate jar via:
  - settings value OR environment variable OR common distro path fallback.

Testable Artifacts and Excpected Functions:
- Render path: `~/.cache/strategient-architect/plantuml/<page_id>.svg`
- Validate shows errors without mutating saved PlantUML/metadata.
- Render updates `page.plantuml`, updates preview, and updates clickable canvas.


