Strategient-Architect Implementation - Session 4

<title>
Diagram canvas rendering: QGraphicsView/QGraphicsScene shows clickable nodes + edges derived from PlantUML, supports pan/zoom/selection, and reroutes edges on move

<description>
Make the first real “PlantUML → clickable UI diagram” work: parse a supported subset of PlantUML into nodes/edges, then render them as QGraphicsItems. End state: you can open the sample project, see clickable boxes/lines, drag nodes, and edges visually update (layout saved separately).

Cursor Prompt to initiate session 4 build (mostly autonomous with some human assistance where needed):
<Prompt>
You are working inside `~/projects/strategient-architect`.

Goal:
- Implement DiagramView/DiagramScene using `QGraphicsView` and `QGraphicsScene`.
- Render parsed PlantUML nodes as movable boxes and parsed PlantUML relationships as connecting paths.
- Support pan/zoom, selection, and edge reroute on node move.

Hard constraints:
- Keep model (project model derived from PlantUML) separate from view (QGraphicsItems). No “logic leakage” into the view layer.
- The scene is a projection of the current page’s parsed PlantUML + metadata; edits in the UI update metadata through `DocumentModel` APIs.
- Do NOT rewrite PlantUML text when the user drags nodes; store layout in `page.metadata.layout`.
- No fancy node editing UI yet. Rendering and basic interaction only.

Requirements:
- Add `ui/diagram/DiagramView` and `ui/diagram/DiagramScene`.
- Add `ui/diagram/items/NodeItem` (subclass `QGraphicsObject` for signals) that shows:
  - node display_name (from metadata) or PlantUML alias fallback
  - a subtle badge for test_status (NotRun/Pass/Fail)
- Add `ui/diagram/items/EdgeItem` (`QGraphicsPathItem`) that connects the center points of nodes initially.
- When a NodeItem is moved, update the corresponding `page.metadata.layout[node_id] = {x,y}`.
- When positions change, EdgeItem paths reroute.
- Add zoom with Ctrl+wheel; pan with middle mouse or Space+drag.

Acceptance:
- Open `assets/sample_project.sarch.json` and see at least 2 nodes and 1 edge on the canvas.
- Drag node: edges follow in real time.
- Save, reload: positions persist.
</Prompt>

Proposed Implmentation of Session 4:
- Add a minimal PlantUML parser for a strict subset (enough for component-style diagrams with aliases + arrows).
- Implement item mapping: node_id (PlantUML alias) → NodeItem*, and reroute edges using those lookups.
- Wire `DocumentModel::currentPageChanged` to rebind the scene.

Container/Kubernetes note (future-proofing):
- None in this session. This is UI + IR projection only.

Testable Artifacts and Excpected Functions:
- Nodes render and are draggable.
- Edges render and reroute on move.
- Positions persist via save/reload.


