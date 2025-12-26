Strategient-Architect Implementation - Session 2

<title>
Repo skeleton: Qt6 C++ studio + Rust engine (C ABI) + build scripts + minimal runnable UI

<description>
Create the new `strategient-architect/` repo foundation next to `novaquant_bt/`. End state is a Qt6 Widgets app that compiles/runs on Linux, links a Rust staticlib via a C ABI boundary, shows a basic docked UI layout, and can preview an SVG rendered from PlantUML (render script only; no auto-downloads).

Cursor Prompt to initiate session 2 build (mostly autonomous with some human assistance where needed):
<Prompt>
You are building a new repo: `~/projects/strategient-architect` (Qt6 C++ desktop app + Rust engine crate).

Goal:
- Establish a hardened, boring, durable foundation that compiles and runs on Ubuntu.
- Minimal UI that proves: Qt app launches, Rust engine links, log panel works, and SVG preview works.

Hard requirements:
- Qt6 Widgets app (C++20) using CMake.
- Repo layout:
  - `engine/` Rust crate that builds a staticlib with a C ABI header in `engine/include/engine.h`.
  - `studio/` Qt6 C++ app.
  - `scripts/` for build and PlantUML render helpers.
- Rust <-> C++ bridge MUST be C ABI (no `cxx` yet).
- Provide one callable Rust function `engine_version()` returning a string via a safe “Rust allocates, Rust frees” pattern.
- UI: `QMainWindow` with:
  - Center: placeholder widget (later DiagramView).
  - Right dock: placeholder Inspector panel.
  - Bottom dock: Run/Log Console panel (read-only text).
  - Left dock: “Agent Chat / Prompt Runner” placeholder (for Cursor CLI integration in later sessions).
  - Optional dock: “PlantUML Preview” (shows an SVG from disk via QtSvg).
- PlantUML:
  - Add `scripts/plantuml_render.sh` that renders `.puml` to `.svg` using an already-installed `plantuml.jar` (do NOT download it).
  - Add one sample `.puml` under `assets/plantuml/sample_infra.puml`.
  - Add one sample rendered output path expectation (but rendering happens via script or later via app).
- Build:
  - Provide `scripts/build_all.sh` that builds Rust (cargo) and Qt (cmake) non-interactively.
  - Provide concise build instructions in `README.md` (do NOT add extra documentation files).

Acceptance checks:
- `scripts/build_all.sh` succeeds on Ubuntu.
- Running the app shows the window and prints `[engine] <version>` in the Run Console.
- PlantUML render script can render `assets/plantuml/sample_infra.puml` into an SVG file.
- If the SVG exists, the app can load and display it in the PlantUML preview dock.

Deliver complete code (no pseudocode). Keep code small and clean.
</Prompt>

Proposed Implmentation of Session 2:
- Establish repository layout and CMake structure.
- Implement safe C ABI string return/free pattern in Rust + C++.
- Implement minimal `MainWindow` with required docks and a “smoke test” call to `engine_version()`.
- Add PlantUML render helper script (manual render) + sample `.puml`.
- Add minimal SVG preview widget in Qt (points at a file path; for now it’s OK to hardcode a default path or load via a File→Open action).

Testable Artifacts and Excpected Functions:
- `scripts/build_all.sh` builds everything.
- `./build/studio/StrategientArchitect` (or similar output) launches.
- Run Console contains an engine version line.
- `scripts/plantuml_render.sh assets/plantuml/sample_infra.puml /tmp/sample_infra.svg` produces an SVG.
- PlantUML preview dock can display `/tmp/sample_infra.svg` (or a configurable path).


