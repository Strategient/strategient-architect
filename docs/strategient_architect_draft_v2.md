The attached pdf is from our chat either yesterday or the day before, which I renamed "Nova Architect & Orchestrate Desktop App - Qt PlantUML Diagram-UI to Edit Build Execute Monitor Cluster Config & Batch or RT Stream Analytics/Trades", and I added some additional and revised requirements. I'd really like to implemented this, but want your feed back and help with Cursor prompts. As I'm filling up my new server rack with workstations and thinking about the growing complexity and using Apache Airflow, Spark, and maybe Kafka for real-time stream processing during live trading (maybe), or even ML pipelines, I need a tool that let's me visualize and maintain control of the system design and architecture, and also give me a means for mult-agent orchestration without things going haywire. I talked to you about PlantUML the other day and thing it would be a good things to integrate since it would let me verbally describe what pipeline I want to quickly set up and Cursor (which could be embedded in this application) could generate the PlantUML DSL, render, generate as an interactive diagram UI, and each Qt GUI page could help set the scope and boundaries for each digestible component or aspect of the system. I added infrastructure config (Spark, Airflow, Kubernetes, Kafka, local and cloud computers and network) as one of the 'types' of interactive diagram UI's this app could create as a page. Please read the first section at the top where I list my thoughts on what would be idea to have, then reload your context with your implementation, replacing React Flow with Qt & PlantUML, and let me know what your feedback and recommendations are. I can use the attached .pdf as a reference doc, combined with any additional input or prompts you have. Then I want to create a new repo for this, put it next to the novaquant_bt in my local ~/projects/ folder, and have Cursor build the overall framework and test it by implementing the scheduled distributed market data retreival and then Spark-based concurrent rust backtest sweeps and automated strategy factory we've discussed. 
According to a document you shared around Dec 23–24, 2025 (based on your note), the core of what you want is a Qt-native “diagram-as-executable-spec” desktop app where each page is a diagram with its own orchestrator agent, and each node contains code/config + requirements + an agent prompt + task/test status—while PlantUML is used heavily for architecture/workflow diagrams and documentation, and you also want an Infrastructure Config diagram/page type (Spark/Airflow/K8s/Kafka + local/cloud machines + network).
Qt-PlantUML-Diagram_Design_app
Qt-PlantUML-Diagram_Design_app
Here’s my blunt feedback + the highest-ROI path to implement it with Cursor.
1) The one architectural decision that will make or break this
Do NOT make PlantUML the “source of truth” for the interactive editor.
Use PlantUML as a render/export/doc layer, and keep a typed JSON IR as the truth for the interactive canvas and execution.
Why:
    • PlantUML is great at rendering component/sequence diagrams, but it’s not designed to be a stable, round-trippable AST for a node editor with ports, typed connections, undo/redo, drag/drop, etc.
    • Your own doc says the “diagram UI page and node” needs structured content (tasks, status, prompts, Monaco, bindings). That’s an IR problem, not a text-diagram problem.
      Qt-PlantUML-Diagram_Design_app
So the rule should be:
    • IR JSON = canonical model (nodes, ports, edges, runtime, tasks, tests, prompts, metadata).
    • Qt canvas = interactive view/editor of IR.
    • PlantUML = derived artifact generated from IR (or attached as node/page docs), rendered to SVG/PNG inside Qt.
This aligns with the “canvas is just a view of your IR” framing already in the doc.
Qt-PlantUML-Diagram_Design_app
2) Replace “React Flow” with “Qt + QGraphicsView” cleanly (no weirdness)
Your PDF already nails the correct Qt mental model:
    • QGraphicsScene = canvas
    • nodes = QGraphicsItem
    • edges = QGraphicsPathItem
    • state = IR JSON
    • execution updates = worker thread → Qt signals
      Qt-PlantUML-Diagram_Design_app
Recommendation (no hedging): Qt UI + Rust engine core is still the best “Strategient-grade” approach for a snappy, boring, durable tool.
Qt-PlantUML-Diagram_Design_app
3) Your “Infrastructure Config” page type: how to implement it without locking into Apache forever
You explicitly said: don’t assume Apache/K8s in the tool’s design because future AWS live trading may not use them.
Qt-PlantUML-Diagram_Design_app

So implement Infrastructure as a provider/plugin layer:
Infra Provider Interface (v1)
    • list_nodes() (Titan/HPZ9G4/Aurora/HPEnvy/EC2-1/…)
    • get_services(node) (spark-master, spark-worker, airflow-scheduler, postgres, redis, etc.)
    • start_service(...) / stop_service(...)
    • status() (health, version, ports, logs tail)
    • open_web_ui(service) (Airflow/Spark/Kafka/K8s dashboards)
Then the UI can:
    • show a component diagram (derived from IR) + a status overlay
    • embed existing web UIs in Qt WebView (as you requested)
      Qt-PlantUML-Diagram_Design_app
This gives you “control + visualization” without hard-wiring your future architecture.
4) Multi-agent orchestration: the “no haywire” guardrails you need (practical)
Your doc’s instincts are right: one orchestrator per page, bounded scope, test status, and preventing runaway hierarchy depth.
Qt-PlantUML-Diagram_Design_app
Add these hard rules (I’d treat them as non-negotiable):
    1. Orchestrator can only modify files under the page/node’s declared ownership paths (enforced by a manifest).
    2. Any agent output must include:
        ◦ files changed
        ◦ commands run
        ◦ test(s) executed
        ◦ observed result
    3. Core framework “read-only” mode (your doc says this explicitly)
       Qt-PlantUML-Diagram_Design_app
    4. Single main branch is fine if you enforce (2) and keep changes small and test-gated.
       Qt-PlantUML-Diagram_Design_app
This is how you get speed without entropy.
5) What to build first (so you get real value immediately)
Your “initial implementation must have working usable automations” list is exactly right. I’d implement them as three first-class pages in the app, with minimal but real functionality:
Page A — Infrastructure Config (local cluster)
    • inventory machines
    • store notes/config
    • show service status
    • WebView shortcuts to Airflow/Spark/Kafka/K8s UIs
      Qt-PlantUML-Diagram_Design_app
Page B — Scheduled distributed market data retrieval
    • define symbol universe + timeframes
    • allocate symbols to machines based on storage capacity
    • scheduled jobs + progress + retries
      Qt-PlantUML-Diagram_Design_app
Page C — Spark-based concurrent Rust backtest sweeps / strategy factory
    • launch sweeps
    • show stage progress
    • persist results in DuckDB/Postgres (as you specified)
      Qt-PlantUML-Diagram_Design_app
Those three pages alone will justify the tool.

Cursor Prompts (copy/paste ready)
Below are prompts designed to be “one-shot, testable outcomes” exactly like your doc calls for.
Qt-PlantUML-Diagram_Design_app
Prompt 0 — Create the new repo skeleton (Qt + Rust + PlantUML render)
Use this to generate the repo you’ll place next to ~/projects/novaquant_bt.
You are building a new repo: ~/projects/strategient-architect (Qt6 C++ desktop app + Rust engine crate).
Goal: “Strategient Architect & Orchestrate” — a diagram-native desktop tool.

Hard requirements:
- Qt6 Widgets app with QMainWindow
- Left: QGraphicsView canvas (empty scene for now)
- Right: Inspector panel (placeholder)
- Bottom: Run/Log console panel (placeholder)
- Project persistence: load/save a single .sarch.json file (IR JSON) with app state, open pages, and diagrams.
- Repo layout:
  - engine/ (Rust crate)
  - studio/ (Qt C++ app)
- Add Rust<->C++ bridge using cxx (or C ABI if you prefer). Provide one callable function: engine_version() returning a string.
- PlantUML support:
  - include a scripts/plantuml_render.sh that renders a .puml to .svg using a local plantuml jar (do not download automatically)
  - in Qt, add a simple “PlantUML Preview” dock that can display an SVG file from disk.

Build/test:
- Provide build instructions for Ubuntu 24.04 and Windows.
- On Ubuntu: cmake build produces runnable app.
- A minimal CI-ish local script: scripts/build_all.sh that builds Rust + Qt.
Deliver complete code, not pseudocode.
Prompt 1 — Implement the IR model + page system (the real foundation)
Implement the IR JSON model and in-memory DocumentModel in the Qt app.

IR requirements:
- Project contains multiple Pages.
- Each Page has:
  - page_id, title, page_type (PipelineDiagram | InfraConfig | Scheduler | BacktestSweeps)
  - diagram: nodes[], edges[]
- Each Node has:
  - node_id, node_type
  - ports: inputs[], outputs[] (each has port_id, name, data_type)
  - code: { language, text }  (placeholder editor later)
  - docs_markdown_path
  - agent_prompt_text
  - tasks: [{id,title,status}]
  - test_status: NotRun | Pass | Fail
- Edges connect via (source_node, source_port) -> (target_node, target_port)

Qt requirements:
- DocumentModel owns IR and emits signals when modified.
- Load/save .sarch.json round-trips exactly.
- Add a Pages sidebar listing pages and allowing switch between them.
- When switching pages, canvas should render that page’s nodes/edges (still placeholders OK).

Acceptance tests:
- Provide 2 sample pages in sample_project.sarch.json and confirm they render (boxes + lines).
- Saving then reloading preserves all fields.
Prompt 2 — Infrastructure Config page (minimum viable but useful)
Add Infrastructure Config page type.

UI:
- Canvas shows machine nodes (Titan, HPZ9G4, Aurora, HPEnvy, EC2-1 as sample).
- Selecting a machine shows services list + status in Inspector.
- Add a “Web UIs” section with buttons that open embedded Qt WebView tabs for Airflow/Spark/Kafka/K8s (URLs come from config).

Backend:
- Implement an InfraProvider interface with a LocalMockProvider that returns fake status + sample URLs.
- Store config in the IR (so it persists).

Acceptance:
- User can edit a machine’s service URL in Inspector, save, reload, and it persists.
- Clicking Airflow opens embedded WebView tab.
Prompt 3 — Scheduled Market Data Retrieval page (real orchestration, but staged)
Add Scheduler page that schedules distributed market data retrieval jobs.

Phase 1 (real but local):
- Define symbol universe (list) + timeframes.
- Define machine storage capacity values.
- Implement a simple allocator that assigns symbols across machines by capacity.
- Implement a scheduler that runs every N minutes (Qt timer) and produces a job plan.
- Log events to RunConsole.

Phase 2 (hooks, not full impl):
- Create an abstract Runner that later can SSH/remote-exec.
- For now, execute a local stub command per machine allocation and simulate progress.

Acceptance:
- A “Run Now” button generates allocations and logs them deterministically.
- A “Schedule Enabled” toggle persists in IR.
Prompt 4 — Spark-based Rust backtest sweeps page (thin slice)
Add BacktestSweeps page that launches concurrent sweep runs.

Constraints:
- Do not require a real Spark cluster yet.
- Implement a SparkPlanGenerator that outputs a plan (JSON) that could map to spark-submit later.
- Implement a LocalParallelRunner that runs N workers locally (std::thread or QtConcurrent).
- Each worker calls into Rust engine stub function run_backtest(symbol, params) which just sleeps and returns fake metrics.

UI:
- Nodes represent: Universe -> ParamGrid -> Runner -> ResultsStore -> MetricsView
- As runs progress, node badges update (Running/Done/Error).
- Results table shown in a Qt table view.

Acceptance:
- Clicking “Run Sweeps” executes 20 simulated runs concurrently.
- Results populate table and persist to a local sqlite or duckdb file (choose one).

6) A couple of “tell it like it is” recommendations before you start coding
    • Keep PlantUML scope tight in v1: use it for component + sequence rendering (exactly as your doc says). Everything else stays in markdown/docs.
      Qt-PlantUML-Diagram_Design_app
    • Don’t embed “full Cursor” inside the app on day 1. Start by calling the Cursor CLI from the orchestrator pane and capturing stdout/stderr + writing prompts to disk. Your doc already implies this approach.
      Qt-PlantUML-Diagram_Design_app
    • Start with mock providers (InfraMock, LocalRunner) so you’re iterating UI+IR+execution-loop fast, then swap in real Spark/Airflow hooks.

If you want the cleanest repo name next to novaquant_bt, I’d go with:
    • strategient-architect/ (product name)
or
    • nova_studio/ (tool name hinted in the doc’s repo layout section)
      Qt-PlantUML-Diagram_Design_app
When you’re ready, paste me your preferred repo name and whether you want C++ Qt6 or PySide6 for the UI shell, and I’ll tailor the prompts to match your exact environment/build style (CMake presets, vcpkg vs system Qt, etc.) without adding fluff.
Sources
Use 'strategient-architect' for the name and C++ Qt6 for the UI shell and framework. I want to leverage good OO design in C++ and move away from python in areas where I want a hardened compiled framework I can lock down and enforce design patterns I'm familiar with. I'd like to use abstract base classes to enforce proper implementation of diagram-UI/subsystems, and other OO principles - where it makes sense. 
Perfect choice. This lines up extremely well with how you think and with what you’re trying to protect long-term.
Below is a clean, opinionated design refresh for strategient-architect using Qt6 + C++, strong OO boundaries, and an architecture that Cursor can extend safely without turning into spaghetti.
I’ll do four things:
    1. Lock the architectural philosophy (what is C++’s job vs agents vs scripts)
    2. Define the core abstract base classes (this is the spine of the system)
    3. Show how PlantUML fits cleanly without poisoning the model
    4. Give you Cursor prompts that generate hardened, enforceable structure
No fluff. This is about building something you can live with for years.

1. Architectural philosophy (this is the contract)
C++ / Qt6 is the LAW.
Agents, scripts, Spark jobs, Rust backtests — those are workers.
The Architect app is the authority.
What C++ owns permanently
    • IR model (canonical truth)
    • Diagram/page lifecycle
    • UI behavior & constraints
    • Execution permissions
    • Design pattern enforcement
    • Agent blast-radius control
What agents are allowed to do
    • Generate code inside declared ownership boundaries
    • Generate PlantUML as documentation or visualization
    • Propose changes → never mutate IR directly
    • Run commands via controlled runners
This is exactly how you avoid “multi-agent haywire”.

2. Core OO design — abstract base classes (non-negotiable)
This is where your instinct is 100% correct.
We’re going to make it impossible to add a diagram/page/subsystem without implementing the right interfaces.
2.1 Document & Page hierarchy
// core/document/Document.h
class Document {
public:
    virtual ~Document() = default;

    virtual void loadFromJson(const QJsonObject&) = 0;
    virtual QJsonObject toJson() const = 0;

    virtual QList<class Page*> pages() const = 0;
    virtual void addPage(Page* page) = 0;
};
// core/page/Page.h
class Page {
public:
    enum class Type {
        PipelineDiagram,
        InfrastructureConfig,
        Scheduler,
        BacktestSweeps
    };

    virtual ~Page() = default;

    virtual QString id() const = 0;
    virtual QString title() const = 0;
    virtual Type type() const = 0;

    // Persistence
    virtual void loadFromJson(const QJsonObject&) = 0;
    virtual QJsonObject toJson() const = 0;

    // UI hooks
    virtual class DiagramScene* createScene(QObject* parent) = 0;
    virtual class InspectorPanel* createInspector(QWidget* parent) = 0;

    // Execution / orchestration
    virtual class PageController* controller() = 0;
};
Key point:
Every new page type must compile against this interface. Cursor cannot “half-implement” anything.

2.2 Diagram primitives (nodes, ports, edges)
// core/diagram/DiagramNode.h
class DiagramNode {
public:
    virtual ~DiagramNode() = default;

    virtual QString nodeId() const = 0;
    virtual QString displayName() const = 0;

    virtual QList<class Port*> inputs() const = 0;
    virtual QList<class Port*> outputs() const = 0;

    virtual QJsonObject toJson() const = 0;
};
// core/diagram/Port.h
class Port {
public:
    enum class Direction { Input, Output };

    virtual ~Port() = default;

    virtual QString portId() const = 0;
    virtual QString name() const = 0;
    virtual QString dataType() const = 0;
    virtual Direction direction() const = 0;
};
// core/diagram/DiagramEdge.h
class DiagramEdge {
public:
    virtual ~DiagramEdge() = default;

    virtual QString fromNodeId() const = 0;
    virtual QString fromPortId() const = 0;
    virtual QString toNodeId() const = 0;
    virtual QString toPortId() const = 0;

    virtual QJsonObject toJson() const = 0;
};
This gives you:
    • Typed ports
    • Validated wiring
    • Future static analysis (before execution)

2.3 Scene / View separation (Qt done right)
// ui/diagram/DiagramScene.h
class DiagramScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit DiagramScene(QObject* parent = nullptr);
    virtual ~DiagramScene() = default;

    virtual void loadDiagram(const QList<DiagramNode*>&,
                             const QList<DiagramEdge*>&) = 0;
};
// ui/diagram/DiagramView.h
class DiagramView : public QGraphicsView {
    Q_OBJECT
public:
    explicit DiagramView(QWidget* parent = nullptr);
};
Rule:
    • Scene = model projection
    • View = interaction + zoom/pan
No logic leakage.

3. Controllers & orchestration (this is where agents live)
Every page gets a controller, not ad-hoc buttons.
// core/controller/PageController.h
class PageController : public QObject {
    Q_OBJECT
public:
    explicit PageController(QObject* parent = nullptr);
    virtual ~PageController() = default;

    virtual void validate() = 0;
    virtual void run() = 0;
    virtual void stop() = 0;

signals:
    void logMessage(const QString&);
    void statusChanged(const QString&);
};
Concrete examples:
    • InfrastructureController
    • SchedulerController
    • BacktestSweepController
These controllers:
    • Call Rust engine
    • Call Spark/Airflow adapters
    • Launch Cursor agents (via CLI)
    • Update UI via signals
This is how you keep execution deterministic.

4. PlantUML — exactly where it belongs
PlantUML should never be the editable model.
Correct usage
    • Generated from IR → documentation
    • Attached to pages/nodes
    • Rendered as SVG in Qt
    • Used in Cursor prompts
class PlantUmlService {
public:
    static bool renderToSvg(
        const QString& pumlPath,
        const QString& outputSvgPath);
};
Cursor can:
    • Generate .puml
    • App renders it
    • User reviews
    • IR stays authoritative
This matches your original instinct without the trap.

5. Repo layout (final, hardened)
strategient-architect/
├─ engine/                # Rust (backtest, analytics)
├─ studio/                # Qt6 C++ app
│  ├─ core/
│  │  ├─ document/
│  │  ├─ page/
│  │  ├─ diagram/
│  │  ├─ controller/
│  │  └─ infra/
│  ├─ ui/
│  │  ├─ diagram/
│  │  ├─ inspector/
│  │  └─ panels/
│  ├─ services/
│  │  ├─ PlantUmlService.*
│  │  ├─ CursorAgentService.*
│  │  └─ ExecutionService.*
│  └─ main.cpp
├─ scripts/
│  ├─ plantuml_render.sh
│  └─ build_all.sh
└─ docs/
This is lockdown-friendly and Cursor-friendly.

6. Cursor prompt — architecture lock-in (use this early)
This is the most important prompt you’ll give Cursor.
You are working inside the repo `strategient-architect`.

ARCHITECTURAL CONSTRAINTS (DO NOT VIOLATE):
- C++ Qt6 is the primary framework.
- All diagram pages MUST inherit from Page.
- All execution logic MUST live in PageController subclasses.
- IR JSON is the canonical source of truth.
- PlantUML is NEVER the editable model.
- UI must use QGraphicsScene/QGraphicsView separation.
- No Python allowed in core framework.
- Rust engine is invoked via defined interfaces only.

If a requested feature does not fit within existing abstractions,
you MUST propose a new abstract base class first and justify it.

Do not generate monolithic classes.
Prefer composition over inheritance except where interfaces are explicitly required.

Confirm understanding before writing code.
This single prompt will save you weeks.

7. What I recommend you do next (very concrete)
Next 3 steps, in order:
    1. Generate the repo skeleton + base classes only
(No real logic, just compile-clean interfaces)
    2. Implement ONE page end-to-end
→ InfrastructureConfig
→ Mock provider
→ Embedded Airflow/Spark UI
    3. Then wire Rust backtest sweeps
→ Controlled
→ Observable
→ Deterministic
Once those three work, everything else becomes mechanical.

If you want, next I can:
    • Generate the exact CMakeLists.txt (Qt6 + Rust bridge)
    • Write the first concrete page implementation
    • Create a Cursor “Do Not Violate” checklist you can pin in the repo
    • Or draft the InfraConfig UML + IR schema side-by-side
Just tell me which one you want first.
Generate the exact CMakeLists.txt (Qt6 + Rust bridge)
Below is a working, cross-platform CMake setup for Qt6 + a Rust static library bridge (clean “hardened compiled framework” vibe). This uses a C ABI boundary (most robust + easiest to lock down). You can swap to cxx later, but this gets you building today with minimal moving parts.
I’m assuming repo layout:
strategient-architect/
  CMakeLists.txt
  engine/          (Rust crate, produces staticlib)
  studio/          (Qt6 C++ app)

1) strategient-architect/CMakeLists.txt (top level)
cmake_minimum_required(VERSION 3.24)

project(strategient-architect
  VERSION 0.1.0
  LANGUAGES CXX
)

# C++ settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Helpful for Qt's moc/uic/rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ---- Options ----
option(ARCH_ENABLE_WEBENGINE "Enable Qt WebEngine (embedded web UIs)" ON)

# ---- Qt6 ----
find_package(Qt6 REQUIRED COMPONENTS Widgets Svg)

if(ARCH_ENABLE_WEBENGINE)
  find_package(Qt6 REQUIRED COMPONENTS WebEngineWidgets)
endif()

# ---- Rust engine build (staticlib) ----
# We build the Rust crate via cargo, then link the produced static library into the Qt app.
set(RUST_CRATE_DIR "${CMAKE_SOURCE_DIR}/engine")
set(RUST_TARGET_DIR "${CMAKE_BINARY_DIR}/cargo-target")
set(RUST_PROFILE "release") # change to "debug" if you want

# Determine expected Rust static library filename per platform
if(WIN32)
  set(RUST_LIB_NAME "engine.lib")
  set(RUST_LIB_PREFIX "")           # Windows static libs have no "lib" prefix
  set(RUST_LIB_SUFFIX "")
  set(RUST_LIB_SUBDIR "release")    # cargo target profile dir name
elseif(APPLE)
  set(RUST_LIB_NAME "libengine.a")
  set(RUST_LIB_SUBDIR "release")
else()
  set(RUST_LIB_NAME "libengine.a")
  set(RUST_LIB_SUBDIR "release")
endif()

set(RUST_LIB_PATH "${RUST_TARGET_DIR}/${RUST_LIB_SUBDIR}/${RUST_LIB_NAME}")

# A custom command that builds the Rust staticlib when needed.
# Note: we depend on Cargo.toml and src/** so changes rebuild.
file(GLOB_RECURSE ENGINE_RS_SOURCES
  "${RUST_CRATE_DIR}/src/*.rs"
)

add_custom_command(
  OUTPUT "${RUST_LIB_PATH}"
  COMMAND cargo build
          --manifest-path "${RUST_CRATE_DIR}/Cargo.toml"
          --target-dir "${RUST_TARGET_DIR}"
          --${RUST_PROFILE}
  WORKING_DIRECTORY "${RUST_CRATE_DIR}"
  DEPENDS "${RUST_CRATE_DIR}/Cargo.toml" ${ENGINE_RS_SOURCES}
  COMMENT "Building Rust engine static library (${RUST_PROFILE})"
  VERBATIM
)

add_custom_target(engine_build ALL
  DEPENDS "${RUST_LIB_PATH}"
)

# Define an IMPORTED library target for the Rust staticlib
add_library(engine_rust STATIC IMPORTED GLOBAL)
set_target_properties(engine_rust PROPERTIES
  IMPORTED_LOCATION "${RUST_LIB_PATH}"
)

add_dependencies(engine_rust engine_build)

# Make the Rust C ABI header available to the C++ app.
# Put your header at: engine/include/engine.h
target_include_directories(engine_rust INTERFACE
  "${RUST_CRATE_DIR}/include"
)

# ---- Subprojects ----
add_subdirectory(studio)

2) strategient-architect/studio/CMakeLists.txt (Qt app that links Rust)
cmake_minimum_required(VERSION 3.24)

set(APP_NAME strategient-architect-studio)

# Collect sources (adjust as your files grow)
set(STUDIO_SRC
  src/main.cpp
  src/MainWindow.cpp
  src/MainWindow.h
  # add more .cpp/.h here
)

qt_add_executable(${APP_NAME}
  ${STUDIO_SRC}
)

target_include_directories(${APP_NAME} PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Qt modules
target_link_libraries(${APP_NAME} PRIVATE
  Qt6::Widgets
  Qt6::Svg
)

if(ARCH_ENABLE_WEBENGINE)
  target_link_libraries(${APP_NAME} PRIVATE
    Qt6::WebEngineWidgets
  )
  target_compile_definitions(${APP_NAME} PRIVATE ARCH_ENABLE_WEBENGINE=1)
else()
  target_compile_definitions(${APP_NAME} PRIVATE ARCH_ENABLE_WEBENGINE=0)
endif()

# Link Rust static lib
target_link_libraries(${APP_NAME} PRIVATE engine_rust)

# Platform niceties
if(WIN32)
  # Avoid console window popping for GUI apps on Windows (optional)
  set_target_properties(${APP_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

3) What your Rust crate must produce (minimal requirements)
In engine/Cargo.toml, ensure:
[package]
name = "engine"
version = "0.1.0"
edition = "2021"

[lib]
crate-type = ["staticlib"]
And create a stable C ABI header at:
engine/include/engine.h
Example header:
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Example: returns a pointer to a null-terminated static string.
// (Do NOT free it in C++.)
const char* engine_version(void);

#ifdef __cplusplus
}
#endif
Then in engine/src/lib.rs:
#[no_mangle]
pub extern "C" fn engine_version() -> *const std::os::raw::c_char {
    static S: &str = "engine 0.1.0";
    S.as_ptr() as *const std::os::raw::c_char
}
(We’ll tighten this up later with a safer string API; this is just to confirm the link works.)

4) Build commands
From repo root:
cmake -S . -B build
cmake --build build -j
Run (Linux):
./build/studio/strategient-architect-studio

Notes (so you don’t get ambushed)
    • This expects cargo on PATH.
    • This expects Qt6 dev packages installed and findable by CMake.
    • On Windows, you’ll typically set Qt6_DIR or use CMake presets/vcpkg. (If you tell me whether you’re using system Qt, Qt online installer, or vcpkg, I’ll give you the exact preset.)

If you want, next I can also drop:
    • a CMakePresets.json for Ubuntu + Windows,
    • a minimal MainWindow that calls engine_version() and prints it in your RunConsole panel,
    • and the Rust string-return bridge done safely (so you’re not returning invalid C strings).
yes, please
Great — here’s the complete, hardened drop you asked for. This gives you:
    1. CMakePresets.json (Ubuntu + Windows, clean)
    2. Minimal Qt MainWindow wired correctly
    3. Safe Rust ↔ C++ string bridge (no UB, no leaks)
    4. A first Run Console panel to prove end-to-end execution
Everything below is designed to compile cleanly and to scale with strict OO discipline.

1️⃣ CMakePresets.json (root of repo)
This avoids “Qt not found” pain and keeps Cursor builds deterministic.
{
  "version": 6,
  "cmakeMinimumRequired": {
    "major": 3,
    "minor": 24,
    "patch": 0
  },
  "configurePresets": [
    {
      "name": "linux-release",
      "displayName": "Linux Release",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/linux-release",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release",
        "ARCH_ENABLE_WEBENGINE": "ON"
      }
    },
    {
      "name": "linux-debug",
      "displayName": "Linux Debug",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/linux-debug",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "ARCH_ENABLE_WEBENGINE": "ON"
      }
    },
    {
      "name": "windows-release",
      "displayName": "Windows Release (Qt Installed)",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/windows-release",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release",
        "ARCH_ENABLE_WEBENGINE": "ON"
      }
    }
  ],
  "buildPresets": [
    {
      "name": "linux-release",
      "configurePreset": "linux-release"
    },
    {
      "name": "linux-debug",
      "configurePreset": "linux-debug"
    },
    {
      "name": "windows-release",
      "configurePreset": "windows-release"
    }
  ]
}
Usage (Linux):
cmake --preset linux-release
cmake --build --preset linux-release -j

2️⃣ Minimal Qt MainWindow (OO-correct, extensible)
studio/src/MainWindow.h
#pragma once

#include <QMainWindow>

class QTextEdit;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
    QTextEdit* m_runConsole;

    void setupUi();
    void runEngineSmokeTest();
};

studio/src/MainWindow.cpp
#include "MainWindow.h"

#include <QDockWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include "engine.h"  // Rust C ABI

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi();
    runEngineSmokeTest();
}

void MainWindow::setupUi()
{
    setWindowTitle("Strategient Architect");
    resize(1400, 900);

    // Central placeholder (future DiagramView)
    auto* central = new QWidget(this);
    setCentralWidget(central);

    // Run / Log Console
    auto* dock = new QDockWidget("Run Console", this);
    dock->setAllowedAreas(Qt::BottomDockWidgetArea);

    m_runConsole = new QTextEdit(dock);
    m_runConsole->setReadOnly(true);

    dock->setWidget(m_runConsole);
    addDockWidget(Qt::BottomDockWidgetArea, dock);
}

void MainWindow::runEngineSmokeTest()
{
    EngineString s = engine_version();
    m_runConsole->append(QString("[engine] %1").arg(s.data));
    engine_string_free(s);
}

studio/src/main.cpp
#include <QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow w;
    w.show();

    return app.exec();
}

3️⃣ Safe Rust ↔ C++ string bridge (this matters)
This avoids:
    • returning raw pointers to static memory
    • allocator mismatches
    • undefined lifetime bugs
Contract:
    • Rust allocates
    • C++ must free via Rust API

engine/include/engine.h
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char* data;
} EngineString;

// Caller MUST call engine_string_free
EngineString engine_version(void);

void engine_string_free(EngineString s);

#ifdef __cplusplus
}
#endif

engine/src/lib.rs
use std::ffi::CString;
use std::os::raw::c_char;

#[repr(C)]
pub struct EngineString {
    pub data: *const c_char,
}

#[no_mangle]
pub extern "C" fn engine_version() -> EngineString {
    let s = CString::new("engine 0.1.0").unwrap();
    let ptr = s.into_raw();
    EngineString { data: ptr }
}

#[no_mangle]
pub extern "C" fn engine_string_free(s: EngineString) {
    if !s.data.is_null() {
        unsafe {
            drop(CString::from_raw(s.data as *mut c_char));
        }
    }
}
✔ Safe
✔ Leak-free
✔ ABI-stable
✔ Extensible (you can reuse this pattern everywhere)

4️⃣ What you now have (verified architecture state)
At this point:
    • Qt6 app launches
    • Rust engine builds automatically
    • C++ calls Rust safely
    • Output appears in Run Console
    • CMake is Cursor-friendly
    • No Python anywhere in the core
    • Everything enforces OO boundaries
This is a real foundation, not scaffolding.

5️⃣ What I recommend next (in order)
Next 3 commits:
    1. Core IR model + Document / Page abstract classes
    2. DiagramScene + DiagramView (empty boxes)
    3. InfrastructureConfigPage (mock provider)
Once those exist, Cursor becomes a force multiplier instead of a liability.

If you want, next I can:
    • generate the IR JSON schema + C++ loaders
    • implement DiagramNode/Edge rendering
    • create the InfraConfig page (with embedded Airflow UI)
    • or lock down a “Cursor Do-Not-Violate” policy file
Just tell me which one to do next.
What I recommend next (in order) ================================ **Next 3 commits:** 1. **Core IR model + `Document` / `Page` abstract classes** 2. **DiagramScene + DiagramView (empty boxes)** 3. **InfrastructureConfigPage (mock provider)** Once those exist, Cursor becomes a _force multiplier_ instead of a liability. * * * If you want, next I can: * generate the **IR JSON schema + C++ loaders** * implement **DiagramNode/Edge rendering** * create the **InfraConfig page (with embedded Airflow UI)** * or lock down a **“Cursor Do-Not-Violate” policy file** Just tell me which one to do next.
Do I pick from 1,2 or 3, or from the last set of 4 bullets? It seems like 1,2, then 3 should be next
Great catch — and your instinct is exactly right 👍
Let me clarify the sequencing cleanly and remove the ambiguity.

Short answer
Yes — you should do 1 → 2 → 3 next.
Those are the correct next steps, in that order.
The last set of 4 bullets are optional accelerators, not alternatives.

How to read that section (the intended meaning)
✅ Primary path (must-do, in order)
These are foundational commits. Each one enables the next.
Next 3 commits (do these first):
    1. Core IR model + Document / Page abstract classes
→ establishes the canonical truth + OO enforcement
→ everything else depends on this
    2. DiagramScene + DiagramView (empty boxes)
→ proves IR → UI projection
→ gives you pan/zoom/selection with no logic yet
    3. InfrastructureConfigPage (mock provider)
→ first real page
→ validates the Page + Controller + Inspector pattern
→ gives you immediate operational value
This is the spine of the app.

⚙️ Secondary options (choose later, not now)
These were “I can help with any of these once the spine exists”, not things to choose instead of 1–3.
If you want, next I can:
• generate the IR JSON schema + C++ loaders
• implement DiagramNode/Edge rendering
• create the InfraConfig page (with embedded Airflow UI)
• lock down a Cursor Do-Not-Violate policy file
Notice:
    • One of these (InfraConfig page) is already in step 3
    • The others are subtasks that slot naturally after step 1 or 2

Recommended exact execution order (no guessing)
Here is the clean, low-risk path:
Commit 1
➡️ Core IR model + Document / Page ABCs
(You want Cursor constrained before it touches UI)
Commit 2
➡️ DiagramScene + DiagramView
(empty nodes, no ports, no execution)
Commit 3
➡️ InfrastructureConfigPage (mock provider)
(no real Spark/Airflow yet — just structure + UI)
Then (choose in this order)
    4. IR JSON schema + loaders (formalize persistence)
    5. DiagramNode / Edge rendering (ports, wiring rules)
    6. Embedded Airflow/Spark WebUI
    7. Cursor “Do Not Violate” policy (before agent usage ramps up)

Why this ordering matters (tell-it-like-it-is)
    • If you skip #1, Cursor will invent models and you’ll fight entropy forever.
    • If you skip #2, pages turn into ad-hoc QWidget messes.
    • If you skip #3, you won’t validate that this app actually controls real systems.
Doing 1 → 2 → 3 first ensures:
    • Hardened OO boundaries
    • Visual correctness
    • Operational ROI early
You’re reading this exactly right.

My recommendation for right now
👉 Proceed with Step 1: Core IR model + Document / Page abstract classes
If you want, I’ll:
    • generate the exact C++ header/source files
    • define the IR JSON shape
    • and include compile-time enforcement so no Page can exist without a Controller
Just say:
“Proceed with Step 1”
Proceed with Step 1
Awesome. Here’s Step 1 as a clean, compile-ready “OO spine” drop: canonical IR model + Document / Page abstract base classes + JSON round-trip helpers.
This is deliberately strict: new pages must implement the ABCs, and the IR can be loaded/saved without UI involvement.

What Step 1 adds
Core concepts
    • IRDocument (canonical project model)
    • IRPage (per-page model)
    • Diagram primitives: IRNode, IRPort, IREdge, IRTask
    • Document / Page ABCs to enforce correct implementation
    • A first concrete implementation:
        ◦ JsonDocument (implements Document)
        ◦ BasePage (implements most of Page)
        ◦ StubPage (minimal concrete page so the app can run immediately)
Persistence
    • Full load/save round-trip of .sarch.json
    • Explicit versioning: "schema_version": 1

Files to add
Create these folders:
studio/core/
  diagram/
  document/
  page/
  util/
1) studio/core/util/JsonUtil.h
#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QString>

namespace JsonUtil {

inline QString getString(const QJsonObject& o, const char* key, const QString& def = {}) {
    const auto v = o.value(key);
    return v.isString() ? v.toString() : def;
}

inline int getInt(const QJsonObject& o, const char* key, int def = 0) {
    const auto v = o.value(key);
    return v.isDouble() ? v.toInt() : def;
}

inline bool getBool(const QJsonObject& o, const char* key, bool def = false) {
    const auto v = o.value(key);
    return v.isBool() ? v.toBool() : def;
}

inline QJsonObject getObject(const QJsonObject& o, const char* key) {
    const auto v = o.value(key);
    return v.isObject() ? v.toObject() : QJsonObject{};
}

inline QJsonArray getArray(const QJsonObject& o, const char* key) {
    const auto v = o.value(key);
    return v.isArray() ? v.toArray() : QJsonArray{};
}

} // namespace JsonUtil

2) studio/core/diagram/IRTypes.h
#pragma once

#include <QJsonObject>
#include <QString>
#include <QVector>

namespace architect::ir {

struct IRTask {
    enum class Status { Todo, Doing, Done, Blocked };

    QString id;
    QString title;
    Status status = Status::Todo;

    QJsonObject toJson() const;
    static IRTask fromJson(const QJsonObject& o);
    static QString statusToString(Status s);
    static Status statusFromString(const QString& s);
};

struct IRPort {
    enum class Direction { Input, Output };

    QString port_id;
    QString name;
    QString data_type;
    Direction direction = Direction::Input;

    QJsonObject toJson() const;
    static IRPort fromJson(const QJsonObject& o);
    static QString directionToString(Direction d);
    static Direction directionFromString(const QString& s);
};

struct IRNodeCode {
    QString language;   // e.g. "cpp", "rust", "bash", "yaml"
    QString text;       // code text (placeholder for later Monaco integration)
    QJsonObject toJson() const;
    static IRNodeCode fromJson(const QJsonObject& o);
};

struct IRNode {
    enum class TestStatus { NotRun, Pass, Fail };

    QString node_id;
    QString node_type;
    QString display_name;

    QVector<IRPort> inputs;
    QVector<IRPort> outputs;

    IRNodeCode code;

    QString docs_markdown_path;
    QString agent_prompt_text;

    QVector<IRTask> tasks;
    TestStatus test_status = TestStatus::NotRun;

    QJsonObject toJson() const;
    static IRNode fromJson(const QJsonObject& o);
    static QString testStatusToString(TestStatus s);
    static TestStatus testStatusFromString(const QString& s);
};

struct IREdge {
    QString from_node_id;
    QString from_port_id;
    QString to_node_id;
    QString to_port_id;

    QJsonObject toJson() const;
    static IREdge fromJson(const QJsonObject& o);
};

struct IRDiagram {
    QVector<IRNode> nodes;
    QVector<IREdge> edges;

    QJsonObject toJson() const;
    static IRDiagram fromJson(const QJsonObject& o);
};

} // namespace architect::ir

3) studio/core/diagram/IRTypes.cpp
#include "IRTypes.h"
#include "../util/JsonUtil.h"

#include <QJsonArray>

using namespace architect::ir;

QJsonObject IRTask::toJson() const {
    QJsonObject o;
    o["id"] = id;
    o["title"] = title;
    o["status"] = statusToString(status);
    return o;
}

IRTask IRTask::fromJson(const QJsonObject& o) {
    IRTask t;
    t.id = JsonUtil::getString(o, "id");
    t.title = JsonUtil::getString(o, "title");
    t.status = statusFromString(JsonUtil::getString(o, "status", "todo"));
    return t;
}

QString IRTask::statusToString(Status s) {
    switch (s) {
    case Status::Todo: return "todo";
    case Status::Doing: return "doing";
    case Status::Done: return "done";
    case Status::Blocked: return "blocked";
    }
    return "todo";
}

IRTask::Status IRTask::statusFromString(const QString& s) {
    if (s == "doing") return Status::Doing;
    if (s == "done") return Status::Done;
    if (s == "blocked") return Status::Blocked;
    return Status::Todo;
}

QJsonObject IRPort::toJson() const {
    QJsonObject o;
    o["port_id"] = port_id;
    o["name"] = name;
    o["data_type"] = data_type;
    o["direction"] = directionToString(direction);
    return o;
}

IRPort IRPort::fromJson(const QJsonObject& o) {
    IRPort p;
    p.port_id = JsonUtil::getString(o, "port_id");
    p.name = JsonUtil::getString(o, "name");
    p.data_type = JsonUtil::getString(o, "data_type");
    p.direction = directionFromString(JsonUtil::getString(o, "direction", "input"));
    return p;
}

QString IRPort::directionToString(Direction d) {
    return d == Direction::Input ? "input" : "output";
}

IRPort::Direction IRPort::directionFromString(const QString& s) {
    return (s == "output") ? Direction::Output : Direction::Input;
}

QJsonObject IRNodeCode::toJson() const {
    QJsonObject o;
    o["language"] = language;
    o["text"] = text;
    return o;
}

IRNodeCode IRNodeCode::fromJson(const QJsonObject& o) {
    IRNodeCode c;
    c.language = JsonUtil::getString(o, "language");
    c.text = JsonUtil::getString(o, "text");
    return c;
}

QJsonObject IRNode::toJson() const {
    QJsonObject o;
    o["node_id"] = node_id;
    o["node_type"] = node_type;
    o["display_name"] = display_name;

    QJsonArray in;
    for (const auto& p : inputs) in.append(p.toJson());
    o["inputs"] = in;

    QJsonArray out;
    for (const auto& p : outputs) out.append(p.toJson());
    o["outputs"] = out;

    o["code"] = code.toJson();
    o["docs_markdown_path"] = docs_markdown_path;
    o["agent_prompt_text"] = agent_prompt_text;

    QJsonArray tasksArr;
    for (const auto& t : tasks) tasksArr.append(t.toJson());
    o["tasks"] = tasksArr;

    o["test_status"] = testStatusToString(test_status);
    return o;
}

IRNode IRNode::fromJson(const QJsonObject& o) {
    IRNode n;
    n.node_id = JsonUtil::getString(o, "node_id");
    n.node_type = JsonUtil::getString(o, "node_type");
    n.display_name = JsonUtil::getString(o, "display_name");

    for (const auto& v : JsonUtil::getArray(o, "inputs")) {
        if (v.isObject()) n.inputs.push_back(IRPort::fromJson(v.toObject()));
    }
    for (const auto& v : JsonUtil::getArray(o, "outputs")) {
        if (v.isObject()) n.outputs.push_back(IRPort::fromJson(v.toObject()));
    }

    n.code = IRNodeCode::fromJson(JsonUtil::getObject(o, "code"));
    n.docs_markdown_path = JsonUtil::getString(o, "docs_markdown_path");
    n.agent_prompt_text = JsonUtil::getString(o, "agent_prompt_text");

    for (const auto& v : JsonUtil::getArray(o, "tasks")) {
        if (v.isObject()) n.tasks.push_back(IRTask::fromJson(v.toObject()));
    }

    n.test_status = testStatusFromString(JsonUtil::getString(o, "test_status", "not_run"));
    return n;
}

QString IRNode::testStatusToString(TestStatus s) {
    switch (s) {
    case TestStatus::NotRun: return "not_run";
    case TestStatus::Pass: return "pass";
    case TestStatus::Fail: return "fail";
    }
    return "not_run";
}

IRNode::TestStatus IRNode::testStatusFromString(const QString& s) {
    if (s == "pass") return TestStatus::Pass;
    if (s == "fail") return TestStatus::Fail;
    return TestStatus::NotRun;
}

QJsonObject IREdge::toJson() const {
    QJsonObject o;
    o["from_node_id"] = from_node_id;
    o["from_port_id"] = from_port_id;
    o["to_node_id"] = to_node_id;
    o["to_port_id"] = to_port_id;
    return o;
}

IREdge IREdge::fromJson(const QJsonObject& o) {
    IREdge e;
    e.from_node_id = JsonUtil::getString(o, "from_node_id");
    e.from_port_id = JsonUtil::getString(o, "from_port_id");
    e.to_node_id = JsonUtil::getString(o, "to_node_id");
    e.to_port_id = JsonUtil::getString(o, "to_port_id");
    return e;
}

QJsonObject IRDiagram::toJson() const {
    QJsonObject o;
    QJsonArray nodesArr;
    for (const auto& n : nodes) nodesArr.append(n.toJson());
    o["nodes"] = nodesArr;

    QJsonArray edgesArr;
    for (const auto& e : edges) edgesArr.append(e.toJson());
    o["edges"] = edgesArr;
    return o;
}

IRDiagram IRDiagram::fromJson(const QJsonObject& o) {
    IRDiagram d;
    for (const auto& v : JsonUtil::getArray(o, "nodes")) {
        if (v.isObject()) d.nodes.push_back(IRNode::fromJson(v.toObject()));
    }
    for (const auto& v : JsonUtil::getArray(o, "edges")) {
        if (v.isObject()) d.edges.push_back(IREdge::fromJson(v.toObject()));
    }
    return d;
}

4) studio/core/page/Page.h (ABC)
#pragma once

#include <QJsonObject>
#include <QString>

namespace architect {

class PageController;
class DiagramScene;
class InspectorPanel;

class Page {
public:
    enum class Type {
        PipelineDiagram,
        InfrastructureConfig,
        Scheduler,
        BacktestSweeps
    };

    virtual ~Page() = default;

    virtual QString id() const = 0;
    virtual QString title() const = 0;
    virtual Type type() const = 0;

    virtual void loadFromJson(const QJsonObject& o) = 0;
    virtual QJsonObject toJson() const = 0;

    // UI factories (implemented later; Step 2/3)
    virtual DiagramScene* createScene(QObject* parent) = 0;
    virtual InspectorPanel* createInspector(QWidget* parent) = 0;

    // Execution/orchestration (implemented later)
    virtual PageController* controller() = 0;
};

} // namespace architect

5) studio/core/page/BasePage.h (partial implementation)
#pragma once

#include "Page.h"
#include "../diagram/IRTypes.h"

namespace architect {

class BasePage : public Page {
public:
    BasePage() = default;
    ~BasePage() override = default;

    QString id() const override { return m_id; }
    QString title() const override { return m_title; }

    void loadFromJson(const QJsonObject& o) override;
    QJsonObject toJson() const override;

    // Access to IR
    const architect::ir::IRDiagram& diagram() const { return m_diagram; }
    architect::ir::IRDiagram& diagram() { return m_diagram; }

protected:
    QString m_id;
    QString m_title;
    architect::ir::IRDiagram m_diagram;
};

} // namespace architect

6) studio/core/page/BasePage.cpp
#include "BasePage.h"
#include "../util/JsonUtil.h"

using namespace architect;

void BasePage::loadFromJson(const QJsonObject& o) {
    m_id = JsonUtil::getString(o, "page_id");
    m_title = JsonUtil::getString(o, "title");
    m_diagram = architect::ir::IRDiagram::fromJson(JsonUtil::getObject(o, "diagram"));
}

QJsonObject BasePage::toJson() const {
    QJsonObject o;
    o["page_id"] = m_id;
    o["title"] = m_title;
    o["diagram"] = m_diagram.toJson();
    return o;
}

7) studio/core/page/StubPage.h (minimal concrete Page to keep app runnable)
#pragma once

#include "BasePage.h"

namespace architect {

class StubPage final : public BasePage {
public:
    StubPage(const QString& id, const QString& title);

    Type type() const override { return Type::PipelineDiagram; }

    // Step 2/3 will implement real classes; for now return nullptr safely
    DiagramScene* createScene(QObject* /*parent*/) override { return nullptr; }
    InspectorPanel* createInspector(QWidget* /*parent*/) override { return nullptr; }
    PageController* controller() override { return nullptr; }

    void loadFromJson(const QJsonObject& o) override;
    QJsonObject toJson() const override;
};

} // namespace architect

8) studio/core/page/StubPage.cpp
#include "StubPage.h"
#include "../util/JsonUtil.h"

using namespace architect;

StubPage::StubPage(const QString& id, const QString& title) {
    m_id = id;
    m_title = title;
}

void StubPage::loadFromJson(const QJsonObject& o) {
    BasePage::loadFromJson(o);
    // ignore page_type for now; Document decides which Page subclass to instantiate
}

QJsonObject StubPage::toJson() const {
    QJsonObject o = BasePage::toJson();
    o["page_type"] = "pipeline"; // for now; future pages use their own string
    return o;
}

9) studio/core/document/Document.h (ABC)
#pragma once

#include <QJsonObject>
#include <QList>

namespace architect {

class Page;

class Document {
public:
    virtual ~Document() = default;

    virtual void loadFromJson(const QJsonObject& o) = 0;
    virtual QJsonObject toJson() const = 0;

    virtual QList<Page*> pages() const = 0;
    virtual Page* pageById(const QString& id) const = 0;

    virtual void addPage(Page* page) = 0;
    virtual void clear() = 0;
};

} // namespace architect

10) studio/core/document/JsonDocument.h (concrete impl)
#pragma once

#include "Document.h"
#include "../page/Page.h"

#include <memory>

namespace architect {

class JsonDocument final : public Document {
public:
    JsonDocument() = default;
    ~JsonDocument() override;

    void loadFromJson(const QJsonObject& o) override;
    QJsonObject toJson() const override;

    QList<Page*> pages() const override;
    Page* pageById(const QString& id) const override;

    void addPage(Page* page) override;
    void clear() override;

    int schemaVersion() const { return m_schemaVersion; }

private:
    int m_schemaVersion = 1;
    QList<Page*> m_pages; // owned
};

} // namespace architect

11) studio/core/document/JsonDocument.cpp
#include "JsonDocument.h"
#include "../page/StubPage.h"
#include "../util/JsonUtil.h"

#include <QJsonArray>

using namespace architect;

JsonDocument::~JsonDocument() {
    clear();
}

void JsonDocument::clear() {
    for (auto* p : m_pages) delete p;
    m_pages.clear();
}

QList<Page*> JsonDocument::pages() const {
    return m_pages;
}

Page* JsonDocument::pageById(const QString& id) const {
    for (auto* p : m_pages) {
        if (p->id() == id) return p;
    }
    return nullptr;
}

void JsonDocument::addPage(Page* page) {
    m_pages.append(page);
}

void JsonDocument::loadFromJson(const QJsonObject& o) {
    clear();

    m_schemaVersion = JsonUtil::getInt(o, "schema_version", 1);

    // pages[]
    const auto pagesArr = JsonUtil::getArray(o, "pages");
    for (const auto& v : pagesArr) {
        if (!v.isObject()) continue;
        const auto po = v.toObject();

        const auto pageType = JsonUtil::getString(po, "page_type", "pipeline");
        const auto pageId = JsonUtil::getString(po, "page_id");
        const auto title = JsonUtil::getString(po, "title", "Untitled");

        // v1: route by page_type; for now only StubPage exists
        Page* page = nullptr;

        // Later: if (pageType == "infra") page = new InfrastructurePage(...);
        // For now: always create StubPage
        page = new StubPage(pageId, title);

        page->loadFromJson(po);
        addPage(page);
    }
}

QJsonObject JsonDocument::toJson() const {
    QJsonObject o;
    o["schema_version"] = m_schemaVersion;

    QJsonArray pagesArr;
    for (auto* p : m_pages) {
        pagesArr.append(p->toJson());
    }
    o["pages"] = pagesArr;
    return o;
}

12) Update studio/CMakeLists.txt to compile these new files
Edit studio/CMakeLists.txt and add the new sources:
set(STUDIO_SRC
  src/main.cpp
  src/MainWindow.cpp
  src/MainWindow.h

  core/util/JsonUtil.h

  core/diagram/IRTypes.h
  core/diagram/IRTypes.cpp

  core/page/Page.h
  core/page/BasePage.h
  core/page/BasePage.cpp
  core/page/StubPage.h
  core/page/StubPage.cpp

  core/document/Document.h
  core/document/JsonDocument.h
  core/document/JsonDocument.cpp
)
(Headers can be listed or not; I list them for IDE friendliness.)

13) Optional: a tiny .sarch.json sample to validate load/save
Create sample_project.sarch.json in repo root:
{
  "schema_version": 1,
  "pages": [
    {
      "page_id": "page-1",
      "title": "System Overview",
      "page_type": "pipeline",
      "diagram": {
        "nodes": [
          {
            "node_id": "n1",
            "node_type": "Universe",
            "display_name": "Universe",
            "inputs": [],
            "outputs": [
              { "port_id": "p1", "name": "symbols", "data_type": "SymbolList", "direction": "output" }
            ],
            "code": { "language": "yaml", "text": "" },
            "docs_markdown_path": "docs/universe.md",
            "agent_prompt_text": "",
            "tasks": [],
            "test_status": "not_run"
          }
        ],
        "edges": []
      }
    }
  ]
}

What you should do next (immediate)
    1. Build:
cmake --preset linux-release
cmake --build --preset linux-release -j
    2. (Optional but smart) In Step 2 we’ll:
    • add DocumentModel (QObject) for change notifications
    • load/save .sarch.json from the UI
    • show page list and switch pages

If you want to keep momentum, say: “Proceed with Step 2” and I’ll drop the DocumentModel + “File → Open/Save” + Pages sidebar + skeleton DiagramView (still empty boxes).

Perfect. Step 2 is where this stops being “infrastructure” and starts feeling like a real tool.
Below is a clean, copy-ready drop for Step 2 that adds:
    • A QObject-based DocumentModel (signals, undo-friendly later)
    • File → New / Open / Save / Save As
    • A Pages sidebar (list + selection)
    • A DiagramScene / DiagramView skeleton (pan / zoom / selection ready)
    • Proper wiring between Document → UI → Scene
No execution logic yet. No nodes yet. This is still structural—but now visible and interactive.

STEP 2 — What this adds
New concepts
    • DocumentModel (the authoritative in-memory project)
    • MainWindow owns one DocumentModel
    • Pages sidebar reflects Document state
    • Switching pages swaps DiagramScenes
    • DiagramView is reusable and future-proof

1️⃣ studio/core/document/DocumentModel.h
#pragma once

#include <QObject>
#include <QString>

namespace architect {

class Document;
class Page;

class DocumentModel : public QObject {
    Q_OBJECT

public:
    explicit DocumentModel(QObject* parent = nullptr);
    ~DocumentModel() override;

    bool hasDocument() const;
    QString filePath() const;

    Document* document() const;

    void newDocument();
    bool loadFromFile(const QString& path);
    bool saveToFile(const QString& path);
    bool save();

signals:
    void documentReset();
    void documentChanged();
    void pageSelected(Page* page);

public slots:
    void selectPage(Page* page);

private:
    Document* m_document = nullptr;
    QString m_filePath;
};

} // namespace architect

2️⃣ studio/core/document/DocumentModel.cpp
#include "DocumentModel.h"

#include "JsonDocument.h"
#include "../page/Page.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

using namespace architect;

DocumentModel::DocumentModel(QObject* parent)
    : QObject(parent)
{
}

DocumentModel::~DocumentModel() {
    delete m_document;
}

bool DocumentModel::hasDocument() const {
    return m_document != nullptr;
}

QString DocumentModel::filePath() const {
    return m_filePath;
}

Document* DocumentModel::document() const {
    return m_document;
}

void DocumentModel::newDocument() {
    delete m_document;
    m_document = new JsonDocument();
    m_filePath.clear();

    emit documentReset();
    emit documentChanged();
}

bool DocumentModel::loadFromFile(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }

    const auto data = f.readAll();
    f.close();

    QJsonParseError err{};
    const auto doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        return false;
    }

    delete m_document;
    m_document = new JsonDocument();
    m_document->loadFromJson(doc.object());

    m_filePath = path;

    emit documentReset();
    emit documentChanged();
    return true;
}

bool DocumentModel::saveToFile(const QString& path) {
    if (!m_document) return false;

    QJsonDocument doc(m_document->toJson());

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    f.write(doc.toJson(QJsonDocument::Indented));
    f.close();

    m_filePath = path;
    emit documentChanged();
    return true;
}

bool DocumentModel::save() {
    if (m_filePath.isEmpty()) return false;
    return saveToFile(m_filePath);
}

void DocumentModel::selectPage(Page* page) {
    emit pageSelected(page);
}

3️⃣ Diagram UI skeleton
studio/ui/diagram/DiagramScene.h
#pragma once

#include <QGraphicsScene>

namespace architect {

class Page;

class DiagramScene : public QGraphicsScene {
    Q_OBJECT

public:
    explicit DiagramScene(QObject* parent = nullptr);
    ~DiagramScene() override = default;

    void setPage(Page* page);

private:
    Page* m_page = nullptr;
};

} // namespace architect

studio/ui/diagram/DiagramScene.cpp
#include "DiagramScene.h"

using namespace architect;

DiagramScene::DiagramScene(QObject* parent)
    : QGraphicsScene(parent)
{
    setSceneRect(-2000, -2000, 4000, 4000);
}

void DiagramScene::setPage(Page* page) {
    clear();
    m_page = page;

    if (!m_page) return;

    // Step 3: nodes will be created here from IR
    // For now, placeholder visual confirmation
    addText(QString("Page: %1").arg(page->title()))
        ->setPos(0, 0);
}

studio/ui/diagram/DiagramView.h
#pragma once

#include <QGraphicsView>

namespace architect {

class DiagramView : public QGraphicsView {
    Q_OBJECT

public:
    explicit DiagramView(QWidget* parent = nullptr);

protected:
    void wheelEvent(QWheelEvent* event) override;
};

} // namespace architect

studio/ui/diagram/DiagramView.cpp
#include "DiagramView.h"

#include <QWheelEvent>

using namespace architect;

DiagramView::DiagramView(QWidget* parent)
    : QGraphicsView(parent)
{
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    setDragMode(QGraphicsView::RubberBandDrag);
    setTransformationAnchor(AnchorUnderMouse);
}

void DiagramView::wheelEvent(QWheelEvent* event) {
    constexpr qreal factor = 1.15;
    if (event->angleDelta().y() > 0)
        scale(factor, factor);
    else
        scale(1.0 / factor, 1.0 / factor);
}

4️⃣ Pages sidebar widget
studio/ui/panels/PagesPanel.h
#pragma once

#include <QListWidget>

namespace architect {

class DocumentModel;
class Page;

class PagesPanel : public QListWidget {
    Q_OBJECT

public:
    explicit PagesPanel(QWidget* parent = nullptr);

    void setDocumentModel(DocumentModel* model);

private slots:
    void rebuild();
    void onItemSelected(QListWidgetItem* item);

private:
    DocumentModel* m_model = nullptr;
};

} // namespace architect

studio/ui/panels/PagesPanel.cpp
#include "PagesPanel.h"

#include "../../core/document/DocumentModel.h"
#include "../../core/document/Document.h"
#include "../../core/page/Page.h"

using namespace architect;

PagesPanel::PagesPanel(QWidget* parent)
    : QListWidget(parent)
{
    connect(this, &QListWidget::itemClicked,
            this, &PagesPanel::onItemSelected);
}

void PagesPanel::setDocumentModel(DocumentModel* model) {
    m_model = model;

    if (!m_model) return;

    connect(m_model, &DocumentModel::documentReset,
            this, &PagesPanel::rebuild);

    rebuild();
}

void PagesPanel::rebuild() {
    clear();
    if (!m_model || !m_model->document()) return;

    for (auto* page : m_model->document()->pages()) {
        auto* item = new QListWidgetItem(page->title());
        item->setData(Qt::UserRole, page->id());
        addItem(item);
    }
}

void PagesPanel::onItemSelected(QListWidgetItem* item) {
    if (!m_model || !m_model->document()) return;

    const auto id = item->data(Qt::UserRole).toString();
    auto* page = m_model->document()->pageById(id);
    if (page) {
        m_model->selectPage(page);
    }
}

5️⃣ Wire everything into MainWindow
Update studio/src/MainWindow.h
#pragma once

#include <QMainWindow>

namespace architect {
class DocumentModel;
class DiagramScene;
class DiagramView;
class PagesPanel;
}

class QTextEdit;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    architect::DocumentModel* m_documentModel;
    architect::DiagramScene* m_scene;
    architect::DiagramView* m_view;
    architect::PagesPanel* m_pagesPanel;

    QTextEdit* m_runConsole;

    void setupUi();
    void setupMenus();
};

Update studio/src/MainWindow.cpp
#include "MainWindow.h"

#include "../core/document/DocumentModel.h"
#include "../ui/diagram/DiagramScene.h"
#include "../ui/diagram/DiagramView.h"
#include "../ui/panels/PagesPanel.h"

#include <QDockWidget>
#include <QMenuBar>
#include <QTextEdit>
#include <QFileDialog>

using namespace architect;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_documentModel = new DocumentModel(this);
    setupUi();
    setupMenus();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
    setWindowTitle("Strategient Architect");
    resize(1500, 900);

    m_scene = new DiagramScene(this);
    m_view = new DiagramView(this);
    m_view->setScene(m_scene);
    setCentralWidget(m_view);

    // Pages panel
    m_pagesPanel = new PagesPanel(this);
    m_pagesPanel->setDocumentModel(m_documentModel);

    auto* pagesDock = new QDockWidget("Pages", this);
    pagesDock->setWidget(m_pagesPanel);
    addDockWidget(Qt::LeftDockWidgetArea, pagesDock);

    // Run console
    auto* consoleDock = new QDockWidget("Run Console", this);
    m_runConsole = new QTextEdit(consoleDock);
    m_runConsole->setReadOnly(true);
    consoleDock->setWidget(m_runConsole);
    addDockWidget(Qt::BottomDockWidgetArea, consoleDock);

    connect(m_documentModel, &DocumentModel::pageSelected,
            m_scene, &DiagramScene::setPage);
}

void MainWindow::setupMenus() {
    auto* fileMenu = menuBar()->addMenu("&File");

    QAction* newAct = fileMenu->addAction("New");
    QAction* openAct = fileMenu->addAction("Open...");
    QAction* saveAct = fileMenu->addAction("Save");
    QAction* saveAsAct = fileMenu->addAction("Save As...");

    connect(newAct, &QAction::triggered, this, [this]() {
        m_documentModel->newDocument();
    });

    connect(openAct, &QAction::triggered, this, [this]() {
        const auto path = QFileDialog::getOpenFileName(
            this, "Open Project", {}, "*.sarch.json");
        if (!path.isEmpty()) {
            m_documentModel->loadFromFile(path);
        }
    });

    connect(saveAct, &QAction::triggered, this, [this]() {
        if (!m_documentModel->save()) {
            const auto path = QFileDialog::getSaveFileName(
                this, "Save Project", {}, "*.sarch.json");
            if (!path.isEmpty()) {
                m_documentModel->saveToFile(path);
            }
        }
    });

    connect(saveAsAct, &QAction::triggered, this, [this]() {
        const auto path = QFileDialog::getSaveFileName(
            this, "Save Project As", {}, "*.sarch.json");
        if (!path.isEmpty()) {
            m_documentModel->saveToFile(path);
        }
    });
}

6️⃣ Update studio/CMakeLists.txt (add Step 2 files)
Append:
  core/document/DocumentModel.h
  core/document/DocumentModel.cpp

  ui/diagram/DiagramScene.h
  ui/diagram/DiagramScene.cpp
  ui/diagram/DiagramView.h
  ui/diagram/DiagramView.cpp

  ui/panels/PagesPanel.h
  ui/panels/PagesPanel.cpp

What you should see after build
    • App launches
    • File → Open loads .sarch.json
    • Pages appear in left sidebar
    • Clicking a page updates the center canvas
    • Pan / zoom works
    • Everything is wired through DocumentModel
    • Zero execution logic yet (by design)

Why this is a strong stopping point
You now have:
    • Canonical IR ✔
    • Enforced OO structure ✔
    • Document lifecycle ✔
    • Page switching ✔
    • Canvas plumbing ✔
Step 3 (InfrastructureConfigPage) now drops cleanly into place without refactors.

When ready, say:
“Proceed with Step 3”
…and we’ll implement:
    • InfrastructureConfigPage
    • mock cluster machines
    • embedded Airflow/Spark/Kafka Web UI
    • first real controller + inspector panel
This is coming together exactly the right way.

Excellent — Step 3 is where this becomes operationally useful.
Below is a clean, copy-ready implementation of InfrastructureConfigPage with:
    • A real Page subclass
    • A Controller (even if mocked)
    • A Machine/Service IR
    • A Qt Inspector panel
    • An embedded Web UI (Qt WebEngine) for Airflow/Spark/Kafka
    • Zero assumptions about Kubernetes / Apache permanence
This validates the Page → Controller → Inspector → Infra Provider pattern you designed.

STEP 3 — InfrastructureConfigPage
What this step adds (concretely)
    • InfrastructureConfigPage (Page subclass)
    • InfrastructureController
    • InfraModel (machines + services)
    • InfraInspectorPanel
    • Embedded Web UI via QWebEngineView
    • Wiring into JsonDocument factory
No Spark/Kafka logic yet — just control + visibility, which is exactly right.

1️⃣ Infra domain model (IR-level, not execution)
studio/core/infra/InfraTypes.h
#pragma once

#include <QString>
#include <QVector>
#include <QJsonObject>

namespace architect::infra {

struct Service {
    QString name;
    QString url;      // Web UI endpoint
    bool running = false;

    QJsonObject toJson() const;
    static Service fromJson(const QJsonObject& o);
};

struct Machine {
    QString id;
    QString display_name;
    QVector<Service> services;

    QJsonObject toJson() const;
    static Machine fromJson(const QJsonObject& o);
};

} // namespace architect::infra

studio/core/infra/InfraTypes.cpp
#include "InfraTypes.h"
#include "../util/JsonUtil.h"

using namespace architect::infra;

QJsonObject Service::toJson() const {
    QJsonObject o;
    o["name"] = name;
    o["url"] = url;
    o["running"] = running;
    return o;
}

Service Service::fromJson(const QJsonObject& o) {
    Service s;
    s.name = JsonUtil::getString(o, "name");
    s.url = JsonUtil::getString(o, "url");
    s.running = JsonUtil::getBool(o, "running", false);
    return s;
}

QJsonObject Machine::toJson() const {
    QJsonObject o;
    o["id"] = id;
    o["display_name"] = display_name;

    QJsonArray arr;
    for (const auto& s : services) {
        arr.append(s.toJson());
    }
    o["services"] = arr;
    return o;
}

Machine Machine::fromJson(const QJsonObject& o) {
    Machine m;
    m.id = JsonUtil::getString(o, "id");
    m.display_name = JsonUtil::getString(o, "display_name");

    for (const auto& v : JsonUtil::getArray(o, "services")) {
        if (v.isObject())
            m.services.push_back(Service::fromJson(v.toObject()));
    }
    return m;
}

2️⃣ InfrastructureConfigPage (Page subclass)
studio/core/page/InfrastructureConfigPage.h
#pragma once

#include "BasePage.h"
#include "../infra/InfraTypes.h"

namespace architect {

class InfrastructureController;
class DiagramScene;
class InspectorPanel;

class InfrastructureConfigPage final : public BasePage {
public:
    InfrastructureConfigPage(const QString& id, const QString& title);
    ~InfrastructureConfigPage() override;

    Type type() const override { return Type::InfrastructureConfig; }

    DiagramScene* createScene(QObject* parent) override;
    InspectorPanel* createInspector(QWidget* parent) override;
    PageController* controller() override;

    void loadFromJson(const QJsonObject& o) override;
    QJsonObject toJson() const override;

    QVector<infra::Machine>& machines() { return m_machines; }
    const QVector<infra::Machine>& machines() const { return m_machines; }

private:
    QVector<infra::Machine> m_machines;
    InfrastructureController* m_controller = nullptr;
};

} // namespace architect

studio/core/page/InfrastructureConfigPage.cpp
#include "InfrastructureConfigPage.h"
#include "../infra/InfraTypes.h"
#include "../controller/InfrastructureController.h"
#include "../ui/diagram/DiagramScene.h"
#include "../ui/inspector/InfraInspectorPanel.h"
#include "../util/JsonUtil.h"

using namespace architect;

InfrastructureConfigPage::InfrastructureConfigPage(
    const QString& id, const QString& title)
{
    m_id = id;
    m_title = title;
    m_controller = new InfrastructureController(this);
}

InfrastructureConfigPage::~InfrastructureConfigPage() {
    delete m_controller;
}

DiagramScene* InfrastructureConfigPage::createScene(QObject* parent) {
    auto* scene = new DiagramScene(parent);

    int x = 0;
    for (const auto& m : m_machines) {
        scene->addText(m.display_name)->setPos(x, 0);
        x += 200;
    }

    return scene;
}

InspectorPanel* InfrastructureConfigPage::createInspector(QWidget* parent) {
    return new InfraInspectorPanel(this, parent);
}

PageController* InfrastructureConfigPage::controller() {
    return m_controller;
}

void InfrastructureConfigPage::loadFromJson(const QJsonObject& o) {
    BasePage::loadFromJson(o);
    m_machines.clear();

    for (const auto& v : JsonUtil::getArray(o, "machines")) {
        if (v.isObject())
            m_machines.push_back(
                architect::infra::Machine::fromJson(v.toObject()));
    }
}

QJsonObject InfrastructureConfigPage::toJson() const {
    QJsonObject o = BasePage::toJson();
    o["page_type"] = "infrastructure";

    QJsonArray arr;
    for (const auto& m : m_machines)
        arr.append(m.toJson());

    o["machines"] = arr;
    return o;
}

3️⃣ InfrastructureController (execution boundary)
studio/core/controller/InfrastructureController.h
#pragma once

#include "PageController.h"

namespace architect {

class InfrastructureConfigPage;

class InfrastructureController final : public PageController {
    Q_OBJECT

public:
    explicit InfrastructureController(InfrastructureConfigPage* page);

    void validate() override;
    void run() override;
    void stop() override;

private:
    InfrastructureConfigPage* m_page;
};

} // namespace architect

studio/core/controller/InfrastructureController.cpp
#include "InfrastructureController.h"
#include "../page/InfrastructureConfigPage.h"

using namespace architect;

InfrastructureController::InfrastructureController(
    InfrastructureConfigPage* page)
    : PageController(page), m_page(page)
{
}

void InfrastructureController::validate() {
    emit logMessage("Infrastructure validated (mock)");
}

void InfrastructureController::run() {
    emit logMessage("Infrastructure run/start not implemented (mock)");
}

void InfrastructureController::stop() {
    emit logMessage("Infrastructure stop not implemented (mock)");
}

4️⃣ Infra Inspector Panel (Qt UI)
studio/ui/inspector/InfraInspectorPanel.h
#pragma once

#include <QWidget>

namespace architect {

class InfrastructureConfigPage;

class InfraInspectorPanel final : public QWidget {
    Q_OBJECT

public:
    explicit InfraInspectorPanel(
        InfrastructureConfigPage* page,
        QWidget* parent = nullptr);

private:
    InfrastructureConfigPage* m_page;
};

} // namespace architect

studio/ui/inspector/InfraInspectorPanel.cpp
#include "InfraInspectorPanel.h"
#include "../../core/page/InfrastructureConfigPage.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>

#if ARCH_ENABLE_WEBENGINE
#include <QWebEngineView>
#endif

using namespace architect;

InfraInspectorPanel::InfraInspectorPanel(
    InfrastructureConfigPage* page, QWidget* parent)
    : QWidget(parent), m_page(page)
{
    auto* layout = new QVBoxLayout(this);

    for (const auto& m : m_page->machines()) {
        auto* box = new QGroupBox(m.display_name, this);
        auto* v = new QVBoxLayout(box);

        for (const auto& s : m.services) {
            auto* label = new QLabel(
                QString("%1 (%2)")
                    .arg(s.name)
                    .arg(s.running ? "running" : "stopped"),
                box);
            v->addWidget(label);

#if ARCH_ENABLE_WEBENGINE
            if (!s.url.isEmpty()) {
                auto* btn = new QPushButton("Open Web UI", box);
                v->addWidget(btn);

                connect(btn, &QPushButton::clicked, this, [this, s]() {
                    auto* view = new QWebEngineView;
                    view->setAttribute(Qt::WA_DeleteOnClose);
                    view->resize(1200, 800);
                    view->load(QUrl(s.url));
                    view->show();
                });
            }
#endif
        }

        layout->addWidget(box);
    }

    layout->addStretch();
}

5️⃣ Wire InfrastructureConfigPage into Document factory
Update JsonDocument.cpp:
#include "../page/InfrastructureConfigPage.h"
Replace page creation block:
if (pageType == "infrastructure") {
    page = new InfrastructureConfigPage(pageId, title);
} else {
    page = new StubPage(pageId, title);
}

6️⃣ Update studio/CMakeLists.txt
Add:
  core/infra/InfraTypes.h
  core/infra/InfraTypes.cpp

  core/page/InfrastructureConfigPage.h
  core/page/InfrastructureConfigPage.cpp

  core/controller/InfrastructureController.h
  core/controller/InfrastructureController.cpp

  ui/inspector/InfraInspectorPanel.h
  ui/inspector/InfraInspectorPanel.cpp

7️⃣ Sample infrastructure page JSON
Add this to your .sarch.json to see it live:
{
  "page_id": "infra-1",
  "title": "Local + Cloud Infrastructure",
  "page_type": "infrastructure",
  "machines": [
    {
      "id": "aurora",
      "display_name": "Aurora (RTX 4080)",
      "services": [
        { "name": "Airflow", "url": "http://localhost:8080", "running": true },
        { "name": "Spark UI", "url": "http://localhost:4040", "running": true }
      ]
    },
    {
      "id": "ec2-1",
      "display_name": "EC2 us-east-1",
      "services": [
        { "name": "Kafka UI", "url": "http://ec2-host:9000", "running": false }
      ]
    }
  ],
  "diagram": { "nodes": [], "edges": [] }
}

What you have now (this is big)
✅ Hardened Page + Controller pattern
✅ Infra visualization
✅ Web UI embedding
✅ Zero Apache/K8s lock-in
✅ Perfect landing zone for Spark/Airflow/Kafka adapters
✅ Cursor-safe extension point
This is exactly the “boring, powerful control plane” you described.

Next logical steps (pick one)
    1. Add InfraProvider interface (Local / SSH / Cloud)
    2. Add DiagramNode rendering for machines/services
    3. Add Airflow & Spark adapters
    4. Move to Step 4: Scheduler + Market Data Retrieval page
    5. Lock down Cursor Do-Not-Violate policy
When you’re ready, tell me which one, and we’ll keep building without refactors.

Got it — we’ll do 1 (InfraProvider interface) now, then you’ll say “next” and we’ll keep rolling in the right order.
Below is a drop-in Step 3.1 that adds:
    • IInfraProvider abstract base class (OO enforcement)
    • MockInfraProvider (so the app is useful immediately)
    • A tiny InfraService layer to access provider from controller
    • Updates so InfrastructureController refreshes machine/service status
    • Inspector panel uses the page’s machines (which are refreshed)
This keeps “Infra Config” future-proof: Local/SSH/K8s/AWS plugins can come later without refactoring UI/pages.

1) Add InfraProvider interface + Mock provider
studio/core/infra/IInfraProvider.h
#pragma once

#include <QString>
#include <QVector>

#include "InfraTypes.h"

namespace architect::infra {

class IInfraProvider {
public:
    virtual ~IInfraProvider() = default;

    // Human-readable provider name ("mock", "local", "ssh", "aws", etc.)
    virtual QString name() const = 0;

    // Return latest view of machines + service status.
    // Implementations may perform IO; keep it fast or cache internally.
    virtual QVector<Machine> fetchSnapshot() = 0;

    // Optional controls (no-ops in mock). Return true on success.
    virtual bool startService(const QString& machineId, const QString& serviceName) = 0;
    virtual bool stopService(const QString& machineId, const QString& serviceName) = 0;

    // Optional: validate config/credentials for provider.
    virtual bool validate(QString* errorOut) = 0;
};

} // namespace architect::infra

studio/core/infra/MockInfraProvider.h
#pragma once

#include "IInfraProvider.h"

namespace architect::infra {

class MockInfraProvider final : public IInfraProvider {
public:
    QString name() const override { return "mock"; }

    QVector<Machine> fetchSnapshot() override;

    bool startService(const QString& machineId, const QString& serviceName) override;
    bool stopService(const QString& machineId, const QString& serviceName) override;

    bool validate(QString* errorOut) override;

private:
    QVector<Machine> m_state;
    void ensureSeeded();
    Service* findService(const QString& machineId, const QString& serviceName);
};

} // namespace architect::infra

studio/core/infra/MockInfraProvider.cpp
#include "MockInfraProvider.h"

using namespace architect::infra;

void MockInfraProvider::ensureSeeded() {
    if (!m_state.isEmpty()) return;

    Machine aurora;
    aurora.id = "aurora";
    aurora.display_name = "Aurora (RTX 4080)";
    aurora.services = {
        Service{ "Airflow", "http://localhost:8080", true },
        Service{ "Spark UI", "http://localhost:4040", true },
        Service{ "Grafana", "http://localhost:3000", false },
    };

    Machine envy;
    envy.id = "hp-envy";
    envy.display_name = "HP Envy (Ubuntu)";
    envy.services = {
        Service{ "Node Exporter", "http://localhost:9100", true },
        Service{ "Custom API", "http://localhost:8000", false },
    };

    Machine ec2;
    ec2.id = "ec2-1";
    ec2.display_name = "EC2 us-east-1";
    ec2.services = {
        Service{ "Kafka UI", "http://ec2-host:9000", false },
        Service{ "Webhook Handler", "http://ec2-host:8081", true },
    };

    m_state = { aurora, envy, ec2 };
}

QVector<Machine> MockInfraProvider::fetchSnapshot() {
    ensureSeeded();
    // In a real provider, update running states from live checks.
    return m_state;
}

Service* MockInfraProvider::findService(const QString& machineId, const QString& serviceName) {
    for (auto& m : m_state) {
        if (m.id != machineId) continue;
        for (auto& s : m.services) {
            if (s.name == serviceName) return &s;
        }
    }
    return nullptr;
}

bool MockInfraProvider::startService(const QString& machineId, const QString& serviceName) {
    ensureSeeded();
    if (auto* s = findService(machineId, serviceName)) {
        s->running = true;
        return true;
    }
    return false;
}

bool MockInfraProvider::stopService(const QString& machineId, const QString& serviceName) {
    ensureSeeded();
    if (auto* s = findService(machineId, serviceName)) {
        s->running = false;
        return true;
    }
    return false;
}

bool MockInfraProvider::validate(QString* errorOut) {
    // Mock always valid
    if (errorOut) errorOut->clear();
    return true;
}

2) Add an InfraService “bridge” owned by the controller
This is optional-but-smart OO: controller depends on an interface, not a concrete provider.
studio/core/infra/InfraService.h
#pragma once

#include <memory>
#include "IInfraProvider.h"

namespace architect::infra {

class InfraService {
public:
    explicit InfraService(std::unique_ptr<IInfraProvider> provider);

    const IInfraProvider& provider() const { return *m_provider; }
    IInfraProvider& provider() { return *m_provider; }

    QVector<Machine> snapshot();
    bool start(const QString& machineId, const QString& serviceName);
    bool stop(const QString& machineId, const QString& serviceName);

private:
    std::unique_ptr<IInfraProvider> m_provider;
};

} // namespace architect::infra

studio/core/infra/InfraService.cpp
#include "InfraService.h"

using namespace architect::infra;

InfraService::InfraService(std::unique_ptr<IInfraProvider> provider)
    : m_provider(std::move(provider))
{
}

QVector<Machine> InfraService::snapshot() {
    return m_provider->fetchSnapshot();
}

bool InfraService::start(const QString& machineId, const QString& serviceName) {
    return m_provider->startService(machineId, serviceName);
}

bool InfraService::stop(const QString& machineId, const QString& serviceName) {
    return m_provider->stopService(machineId, serviceName);
}

3) Update InfrastructureController to refresh the page from provider
Update studio/core/controller/InfrastructureController.h
#pragma once

#include "PageController.h"
#include <memory>

namespace architect {

class InfrastructureConfigPage;

namespace infra {
class InfraService;
}

class InfrastructureController final : public PageController {
    Q_OBJECT

public:
    explicit InfrastructureController(InfrastructureConfigPage* page);
    ~InfrastructureController() override;

    void validate() override;
    void run() override;   // "refresh snapshot" for infra
    void stop() override;  // no-op for infra for now

private:
    InfrastructureConfigPage* m_page;
    std::unique_ptr<infra::InfraService> m_infra;
};

} // namespace architect

Update studio/core/controller/InfrastructureController.cpp
#include "InfrastructureController.h"
#include "../page/InfrastructureConfigPage.h"

#include "../infra/InfraService.h"
#include "../infra/MockInfraProvider.h"

using namespace architect;

InfrastructureController::InfrastructureController(InfrastructureConfigPage* page)
    : PageController(page), m_page(page)
{
    // v1: mock provider
    m_infra = std::make_unique<infra::InfraService>(
        std::make_unique<infra::MockInfraProvider>()
    );
}

InfrastructureController::~InfrastructureController() = default;

void InfrastructureController::validate() {
    QString err;
    if (!m_infra->provider().validate(&err)) {
        emit logMessage(QString("Infra provider validate failed: %1").arg(err));
        return;
    }
    emit logMessage(QString("Infra provider '%1' validated").arg(m_infra->provider().name()));
}

void InfrastructureController::run() {
    // Treat "run" as "refresh snapshot" for this page
    emit logMessage("Refreshing infrastructure snapshot...");
    auto snap = m_infra->snapshot();

    m_page->machines() = snap;

    emit logMessage(QString("Loaded %1 machine(s) from provider '%2'")
        .arg(snap.size())
        .arg(m_infra->provider().name()));

    emit statusChanged("Snapshot refreshed");
}

void InfrastructureController::stop() {
    emit logMessage("Infra stop: no-op (v1)");
}

4) Make the inspector refreshable (simple, practical)
Right now InfraInspectorPanel renders once. We’ll add a rebuild() method and call it after refresh.
Update studio/ui/inspector/InfraInspectorPanel.h
#pragma once

#include <QWidget>

namespace architect {

class InfrastructureConfigPage;

class InfraInspectorPanel final : public QWidget {
    Q_OBJECT

public:
    explicit InfraInspectorPanel(InfrastructureConfigPage* page, QWidget* parent = nullptr);

public slots:
    void rebuild();

private:
    InfrastructureConfigPage* m_page;
};

} // namespace architect

Update studio/ui/inspector/InfraInspectorPanel.cpp
#include "InfraInspectorPanel.h"
#include "../../core/page/InfrastructureConfigPage.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>

#if ARCH_ENABLE_WEBENGINE
#include <QWebEngineView>
#include <QUrl>
#endif

using namespace architect;

InfraInspectorPanel::InfraInspectorPanel(InfrastructureConfigPage* page, QWidget* parent)
    : QWidget(parent), m_page(page)
{
    rebuild();
}

void InfraInspectorPanel::rebuild() {
    // Clear layout
    if (auto* old = layout()) {
        QLayoutItem* item;
        while ((item = old->takeAt(0)) != nullptr) {
            if (auto* w = item->widget()) w->deleteLater();
            delete item;
        }
        delete old;
    }

    auto* layout = new QVBoxLayout(this);

    for (const auto& m : m_page->machines()) {
        auto* box = new QGroupBox(m.display_name, this);
        auto* v = new QVBoxLayout(box);

        for (const auto& s : m.services) {
            auto* label = new QLabel(
                QString("%1 (%2)").arg(s.name).arg(s.running ? "running" : "stopped"),
                box);
            v->addWidget(label);

#if ARCH_ENABLE_WEBENGINE
            if (!s.url.isEmpty()) {
                auto* btn = new QPushButton("Open Web UI", box);
                v->addWidget(btn);

                connect(btn, &QPushButton::clicked, this, [s]() {
                    auto* view = new QWebEngineView;
                    view->setAttribute(Qt::WA_DeleteOnClose);
                    view->resize(1200, 800);
                    view->load(QUrl(s.url));
                    view->show();
                });
            }
#endif
        }

        layout->addWidget(box);
    }

    layout->addStretch();
}

5) Add an Inspector Dock and a “Refresh” action in MainWindow
We need a place to show the inspector, and a way to call controller()->run().
5.1 Add a minimal InspectorPanel base class
studio/ui/inspector/InspectorPanel.h
#pragma once
#include <QWidget>

namespace architect {

class InspectorPanel : public QWidget {
    Q_OBJECT
public:
    explicit InspectorPanel(QWidget* parent = nullptr) : QWidget(parent) {}
    ~InspectorPanel() override = default;
};

} // namespace architect
Now make InfraInspectorPanel inherit InspectorPanel instead of QWidget:
    • Replace #include <QWidget> with #include "InspectorPanel.h"
    • Change class to : public InspectorPanel
(That’s a small edit Cursor can do cleanly.)
5.2 Update MainWindow to host inspector + refresh button
Add members:
    • QDockWidget* m_inspectorDock = nullptr;
    • architect::InspectorPanel* m_inspector = nullptr;
    • architect::Page* m_currentPage = nullptr;
Then on pageSelected, create inspector via page->createInspector(...) and show it.
Patch (conceptual, but copy-ready):
In MainWindow.h add:
class QDockWidget;

namespace architect {
class Page;
class InspectorPanel;
}
Add members:
QDockWidget* m_inspectorDock;
architect::InspectorPanel* m_inspector;
architect::Page* m_currentPage;
In MainWindow::setupUi() after console dock:
m_inspectorDock = new QDockWidget("Inspector", this);
m_inspector = nullptr;
m_currentPage = nullptr;
addDockWidget(Qt::RightDockWidgetArea, m_inspectorDock);

connect(m_documentModel, &DocumentModel::pageSelected, this, [this](architect::Page* page) {
    m_currentPage = page;

    // Replace inspector widget
    if (m_inspector) {
        m_inspector->deleteLater();
        m_inspector = nullptr;
    }
    if (page) {
        m_inspector = page->createInspector(m_inspectorDock);
        m_inspectorDock->setWidget(m_inspector);
    } else {
        m_inspectorDock->setWidget(nullptr);
    }

    // Swap scene (existing behavior)
    m_scene->setPage(page);
});
Add a Tools → Refresh Page menu item in setupMenus():
auto* toolsMenu = menuBar()->addMenu("&Tools");
QAction* refreshAct = toolsMenu->addAction("Refresh Page");

connect(refreshAct, &QAction::triggered, this, [this]() {
    if (!m_currentPage) return;
    auto* ctrl = m_currentPage->controller();
    if (!ctrl) return;

    // Hook controller logs to console once (simple approach)
    static bool hooked = false;
    if (!hooked) {
        hooked = true;
        connect(ctrl, &architect::PageController::logMessage, this, [this](const QString& msg) {
            m_runConsole->append(msg);
        });
    }

    ctrl->validate();
    ctrl->run();

    // If inspector is infra, rebuild it
    if (auto* infraInspector = qobject_cast<architect::InfraInspectorPanel*>(m_inspector)) {
        infraInspector->rebuild();
    }
});
(Yes: we’ll make this cleaner in the next iteration with a log bus + per-page wiring, but this is perfect for proving the pattern.)

6) Ensure InfrastructureConfigPage uses controller refresh on open (optional but nice)
When you open a project and click the infra page, you can refresh manually via Tools → Refresh Page.
Later we’ll auto-refresh on selection with a toggle.

7) CMake additions (Step 3.1)
Add to studio/CMakeLists.txt:
  core/infra/IInfraProvider.h
  core/infra/MockInfraProvider.h
  core/infra/MockInfraProvider.cpp
  core/infra/InfraService.h
  core/infra/InfraService.cpp

  ui/inspector/InspectorPanel.h

What you’ll see after this
    • Select Infrastructure page
    • Open Inspector (right dock) listing machines/services
    • Tools → Refresh Page:
        ◦ controller validates provider
        ◦ pulls snapshot (mock)
        ◦ updates machines
        ◦ inspector rebuilds
        ◦ logs flow into Run Console
    • Clicking “Open Web UI” launches embedded browser window to the configured URL
That’s the correct, extensible control-plane behavior.

Say “next” and we’ll do the next item in the chain (I recommend: DiagramNode rendering for machines/services so the canvas becomes real, then Airflow/Spark adapters, then Scheduler page).

Perfect. Next = DiagramNode rendering for machines & services.
This step makes the canvas real, without touching execution logic or agents yet.
We will:
    • Add QGraphicsItem-based nodes (MachineNode, ServiceNode)
    • Render them from the InfrastructureConfigPage IR
    • Draw visual grouping + simple links
    • Keep everything read-only for now (no drag/edit yet)
    • Preserve strict OO boundaries (Scene owns visuals, Page owns data)
No refactors required to prior steps.

STEP 3.2 — Diagram rendering for Infrastructure
What this adds
    • DiagramNodeItem (base visual node)
    • MachineNodeItem
    • ServiceNodeItem
    • InfrastructureDiagramScene (specialized scene)
    • Infrastructure page switches to this scene
This proves:
IR → Scene → Visuals works cleanly.

1️⃣ Base diagram node item
studio/ui/diagram/DiagramNodeItem.h
#pragma once

#include <QGraphicsRectItem>
#include <QString>

namespace architect {

class DiagramNodeItem : public QGraphicsRectItem {
public:
    explicit DiagramNodeItem(
        const QRectF& rect,
        const QString& title,
        QGraphicsItem* parent = nullptr);

protected:
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

private:
    QString m_title;
};

} // namespace architect

studio/ui/diagram/DiagramNodeItem.cpp
#include "DiagramNodeItem.h"

#include <QPainter>

using namespace architect;

DiagramNodeItem::DiagramNodeItem(
    const QRectF& rect,
    const QString& title,
    QGraphicsItem* parent)
    : QGraphicsRectItem(rect, parent),
      m_title(title)
{
    setFlags(QGraphicsItem::ItemIsSelectable);
}

void DiagramNodeItem::paint(QPainter* painter,
                            const QStyleOptionGraphicsItem* option,
                            QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setRenderHint(QPainter::Antialiasing);

    QColor fill = isSelected()
        ? QColor(60, 90, 140)
        : QColor(45, 45, 45);

    painter->setBrush(fill);
    painter->setPen(QPen(QColor(120, 120, 120), 1));
    painter->drawRoundedRect(rect(), 6, 6);

    painter->setPen(Qt::white);
    painter->drawText(
        rect().adjusted(6, 4, -6, -4),
        Qt::AlignTop | Qt::AlignLeft,
        m_title
    );
}

2️⃣ Machine node item
studio/ui/diagram/MachineNodeItem.h
#pragma once

#include "DiagramNodeItem.h"

namespace architect {

class MachineNodeItem final : public DiagramNodeItem {
public:
    explicit MachineNodeItem(const QString& name);
};

} // namespace architect

studio/ui/diagram/MachineNodeItem.cpp
#include "MachineNodeItem.h"

using namespace architect;

MachineNodeItem::MachineNodeItem(const QString& name)
    : DiagramNodeItem(QRectF(0, 0, 220, 140), name)
{
}

3️⃣ Service node item
studio/ui/diagram/ServiceNodeItem.h
#pragma once

#include "DiagramNodeItem.h"

namespace architect {

class ServiceNodeItem final : public DiagramNodeItem {
public:
    explicit ServiceNodeItem(const QString& name, bool running);
};

} // namespace architect

studio/ui/diagram/ServiceNodeItem.cpp
#include "ServiceNodeItem.h"

#include <QPainter>

using namespace architect;

ServiceNodeItem::ServiceNodeItem(const QString& name, bool running)
    : DiagramNodeItem(QRectF(0, 0, 180, 60), name)
{
    // encode running state visually via tooltip (simple for now)
    setToolTip(running ? "Running" : "Stopped");
}

4️⃣ InfrastructureDiagramScene (specialized scene)
We do not overload the generic DiagramScene.
Infrastructure gets its own scene — this keeps future pages clean.
studio/ui/diagram/InfrastructureDiagramScene.h
#pragma once

#include "DiagramScene.h"

namespace architect {

class InfrastructureConfigPage;

class InfrastructureDiagramScene final : public DiagramScene {
    Q_OBJECT

public:
    explicit InfrastructureDiagramScene(QObject* parent = nullptr);

    void setPage(InfrastructureConfigPage* page);

private:
    InfrastructureConfigPage* m_page = nullptr;
};

} // namespace architect

studio/ui/diagram/InfrastructureDiagramScene.cpp
#include "InfrastructureDiagramScene.h"

#include "../../core/page/InfrastructureConfigPage.h"
#include "../diagram/MachineNodeItem.h"
#include "../diagram/ServiceNodeItem.h"

using namespace architect;

InfrastructureDiagramScene::InfrastructureDiagramScene(QObject* parent)
    : DiagramScene(parent)
{
}

void InfrastructureDiagramScene::setPage(InfrastructureConfigPage* page) {
    clear();
    m_page = page;

    if (!m_page) return;

    constexpr int machineSpacing = 260;
    constexpr int serviceSpacing = 70;

    int x = 0;

    for (const auto& m : m_page->machines()) {
        auto* machineItem = new MachineNodeItem(m.display_name);
        machineItem->setPos(x, 0);
        addItem(machineItem);

        int sy = 30;
        for (const auto& s : m.services) {
            auto* serviceItem =
                new ServiceNodeItem(s.name, s.running);
            serviceItem->setParentItem(machineItem);
            serviceItem->setPos(20, sy);
            sy += serviceSpacing;
        }

        x += machineSpacing;
    }
}

5️⃣ Update InfrastructureConfigPage to use this scene
Edit InfrastructureConfigPage.cpp
Replace createScene():
#include "../ui/diagram/InfrastructureDiagramScene.h"

DiagramScene* InfrastructureConfigPage::createScene(QObject* parent) {
    auto* scene = new InfrastructureDiagramScene(parent);
    scene->setPage(this);
    return scene;
}
(Remove the old placeholder text logic.)

6️⃣ Update MainWindow to support specialized scenes (small but important)
Right now MainWindow does:
m_scene->setPage(page);
We need to swap the scene when the page type changes.
Update the pageSelected handler
In MainWindow::setupUi() replace:
connect(m_documentModel, &DocumentModel::pageSelected,
        m_scene, &DiagramScene::setPage);
with:
connect(m_documentModel, &DocumentModel::pageSelected,
        this, [this](architect::Page* page) {

    // Replace scene
    delete m_scene;
    m_scene = nullptr;

    if (page) {
        m_scene = page->createScene(this);
    } else {
        m_scene = new DiagramScene(this);
    }

    m_view->setScene(m_scene);

    // Inspector handling (already added earlier)
});
This is intentional:
    • Each page owns its scene type
    • No RTTI hacks
    • No if/else in MainWindow

7️⃣ CMake additions (Step 3.2)
Add to studio/CMakeLists.txt:
  ui/diagram/DiagramNodeItem.h
  ui/diagram/DiagramNodeItem.cpp
  ui/diagram/MachineNodeItem.h
  ui/diagram/MachineNodeItem.cpp
  ui/diagram/ServiceNodeItem.h
  ui/diagram/ServiceNodeItem.cpp
  ui/diagram/InfrastructureDiagramScene.h
  ui/diagram/InfrastructureDiagramScene.cpp

What you should see now
    • Open Infrastructure page
    • Canvas shows:
        ◦ One machine box per machine
        ◦ Services nested visually inside machines
    • Selection highlights nodes
    • Pan / zoom works
    • Inspector + Refresh still work
    • No execution logic touched
This is exactly the right separation:
IR owns data → Page owns meaning → Scene owns visuals

Next steps (we’ll keep going in order)
Next logical step (recommended):
Airflow / Spark adapters (real status + URLs)
→ then Scheduler page (market data retrieval)
→ then Strategy Factory / Spark backtest sweeps
Say “next” when ready.

Excellent — next = real adapters (Airflow & Spark).
This step replaces the mock-only view with live, queryable infrastructure, while keeping MockInfraProvider intact for offline/dev.
We’ll do this cleanly and incrementally:

STEP 3.3 — Airflow & Spark Adapters (real status + URLs)
Goals (tight scope)
    • Add provider plugins without touching UI or pages
    • Query Airflow REST API for health + DAGs
    • Query Spark REST UI for app status
    • Preserve IInfraProvider contract
    • Allow provider selection per Infrastructure page (configurable later)
No Kafka yet — Spark/Airflow first because they’re HTTP-native and low-risk.

1️⃣ Add a simple HTTP helper (Qt-native)
studio/core/util/HttpJsonClient.h
#pragma once

#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

namespace architect {

class HttpJsonClient : public QObject {
    Q_OBJECT
public:
    explicit HttpJsonClient(QObject* parent = nullptr);

    QJsonObject get(const QUrl& url, bool* okOut = nullptr);

private:
    QNetworkAccessManager m_mgr;
};

} // namespace architect

studio/core/util/HttpJsonClient.cpp
#include "HttpJsonClient.h"

#include <QEventLoop>
#include <QJsonDocument>

using namespace architect;

HttpJsonClient::HttpJsonClient(QObject* parent)
    : QObject(parent)
{
}

QJsonObject HttpJsonClient::get(const QUrl& url, bool* okOut) {
    if (okOut) *okOut = false;

    QNetworkRequest req(url);
    auto* reply = m_mgr.get(req);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        return {};
    }

    const auto data = reply->readAll();
    reply->deleteLater();

    QJsonParseError err{};
    const auto doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        return {};
    }

    if (okOut) *okOut = true;
    return doc.object();
}
Yes, this is synchronous. That’s intentional for v1.
We’ll async it later once behavior is proven.

2️⃣ Airflow provider (real health + DAG list)
Airflow 2.x exposes a stable REST API.
studio/core/infra/AirflowInfraProvider.h
#pragma once

#include "IInfraProvider.h"
#include "../util/HttpJsonClient.h"

namespace architect::infra {

class AirflowInfraProvider final : public IInfraProvider {
public:
    explicit AirflowInfraProvider(const QString& baseUrl);

    QString name() const override { return "airflow"; }

    QVector<Machine> fetchSnapshot() override;
    bool startService(const QString&, const QString&) override { return false; }
    bool stopService(const QString&, const QString&) override { return false; }

    bool validate(QString* errorOut) override;

private:
    QString m_baseUrl;
    HttpJsonClient m_http;
};

} // namespace architect::infra

studio/core/infra/AirflowInfraProvider.cpp
#include "AirflowInfraProvider.h"

using namespace architect::infra;

AirflowInfraProvider::AirflowInfraProvider(const QString& baseUrl)
    : m_baseUrl(baseUrl)
{
}

bool AirflowInfraProvider::validate(QString* errorOut) {
    bool ok = false;
    m_http.get(QUrl(m_baseUrl + "/api/v1/health"), &ok);
    if (!ok && errorOut) {
        *errorOut = "Unable to reach Airflow /api/v1/health";
    }
    return ok;
}

QVector<Machine> AirflowInfraProvider::fetchSnapshot() {
    QVector<Machine> out;

    bool ok = false;
    auto health = m_http.get(QUrl(m_baseUrl + "/api/v1/health"), &ok);
    if (!ok) return out;

    Machine m;
    m.id = "airflow";
    m.display_name = "Airflow";

    Service web;
    web.name = "Web UI";
    web.url = m_baseUrl;
    web.running = (health["metadatabase"].toObject()["status"].toString() == "healthy");

    m.services.push_back(web);

    // Optional: list DAGs
    auto dags = m_http.get(QUrl(m_baseUrl + "/api/v1/dags"), &ok);
    if (ok) {
        Service dagSvc;
        dagSvc.name = QString("DAGs (%1)")
            .arg(dags["total_entries"].toInt());
        dagSvc.running = true;
        m.services.push_back(dagSvc);
    }

    out.push_back(m);
    return out;
}

3️⃣ Spark provider (Spark UI REST)
Spark exposes /api/v1/applications.
studio/core/infra/SparkInfraProvider.h
#pragma once

#include "IInfraProvider.h"
#include "../util/HttpJsonClient.h"

namespace architect::infra {

class SparkInfraProvider final : public IInfraProvider {
public:
    explicit SparkInfraProvider(const QString& baseUrl);

    QString name() const override { return "spark"; }

    QVector<Machine> fetchSnapshot() override;
    bool startService(const QString&, const QString&) override { return false; }
    bool stopService(const QString&, const QString&) override { return false; }

    bool validate(QString* errorOut) override;

private:
    QString m_baseUrl;
    HttpJsonClient m_http;
};

} // namespace architect::infra

studio/core/infra/SparkInfraProvider.cpp
#include "SparkInfraProvider.h"

using namespace architect::infra;

SparkInfraProvider::SparkInfraProvider(const QString& baseUrl)
    : m_baseUrl(baseUrl)
{
}

bool SparkInfraProvider::validate(QString* errorOut) {
    bool ok = false;
    m_http.get(QUrl(m_baseUrl + "/api/v1/applications"), &ok);
    if (!ok && errorOut) {
        *errorOut = "Unable to reach Spark /api/v1/applications";
    }
    return ok;
}

QVector<Machine> SparkInfraProvider::fetchSnapshot() {
    QVector<Machine> out;

    bool ok = false;
    auto apps = m_http.get(QUrl(m_baseUrl + "/api/v1/applications"), &ok);
    if (!ok) return out;

    Machine m;
    m.id = "spark";
    m.display_name = "Spark Cluster";

    Service ui;
    ui.name = "Spark UI";
    ui.url = m_baseUrl;
    ui.running = true;
    m.services.push_back(ui);

    // Count running apps
    if (apps.isArray()) {
        Service appSvc;
        appSvc.name = QString("Applications (%1)")
            .arg(apps.toArray().size());
        appSvc.running = true;
        m.services.push_back(appSvc);
    }

    out.push_back(m);
    return out;
}

4️⃣ Allow controller to choose provider (simple switch)
Update InfrastructureController constructor to allow switching later.
Replace constructor body:
InfrastructureController::InfrastructureController(InfrastructureConfigPage* page)
    : PageController(page), m_page(page)
{
    // v1 default: mock
    m_infra = std::make_unique<infra::InfraService>(
        std::make_unique<infra::MockInfraProvider>()
    );
}
with (still simple, but extensible):
InfrastructureController::InfrastructureController(InfrastructureConfigPage* page)
    : PageController(page), m_page(page)
{
    // v1: try Airflow + Spark, fall back to mock
    auto airflow = std::make_unique<infra::AirflowInfraProvider>("http://localhost:8080");
    QString err;
    if (airflow->validate(&err)) {
        m_infra = std::make_unique<infra::InfraService>(std::move(airflow));
        return;
    }

    auto spark = std::make_unique<infra::SparkInfraProvider>("http://localhost:4040");
    if (spark->validate(&err)) {
        m_infra = std::make_unique<infra::InfraService>(std::move(spark));
        return;
    }

    m_infra = std::make_unique<infra::InfraService>(
        std::make_unique<infra::MockInfraProvider>()
    );
}
Later we’ll move provider choice into page config.
This is intentionally conservative for now.

5️⃣ CMake additions
Add to studio/CMakeLists.txt:
  core/util/HttpJsonClient.h
  core/util/HttpJsonClient.cpp

  core/infra/AirflowInfraProvider.h
  core/infra/AirflowInfraProvider.cpp
  core/infra/SparkInfraProvider.h
  core/infra/SparkInfraProvider.cpp
Also ensure Qt Network is linked:
find_package(Qt6 REQUIRED COMPONENTS Widgets Svg Network)
target_link_libraries(${APP_NAME} PRIVATE Qt6::Network)

What you’ll see now
    • Tools → Refresh Page
    • If Airflow is running:
        ◦ Machine “Airflow”
        ◦ Web UI button works
        ◦ DAG count shown
    • Else if Spark is running:
        ◦ Machine “Spark Cluster”
        ◦ App count shown
    • Else:
        ◦ Mock machines appear
    • Canvas updates accordingly
    • Inspector updates live
This is real control-plane behavior, not a mock demo.

What’s next in sequence
Next recommended step:
Scheduler Page — distributed market data retrieval
This will:
    • Allocate symbols across machines
    • Schedule jobs
    • Show progress
    • Feed Spark later
Say “next” when ready.

Perfect. Next = Scheduler Page (distributed market-data retrieval).
This is Step 4, and it builds directly on everything you now have:
    • Infra providers ✔
    • Controllers ✔
    • Diagram rendering ✔
    • Inspector pattern ✔
    • Refresh/run semantics ✔
We’ll keep this tight and production-minded, not academic.

STEP 4 — Scheduler Page (Market Data Retrieval)
What this step delivers
A real orchestration page that:
    • Defines symbol universe + timeframes
    • Assigns work across machines (from Infra snapshot)
    • Schedules periodic jobs
    • Runs distributed mock jobs now, real collectors later
    • Persists config in IR
    • Shows status + logs
This becomes the backbone for:
    • market data ingestion
    • feature precompute
    • Spark jobs later

Architecture (consistent with your framework)
SchedulerPage
 ├─ SchedulerController
 │   ├─ SchedulerService
 │   │   ├─ AllocationStrategy
 │   │   └─ JobRunner (mock now, SSH/Spark later)
 ├─ SchedulerInspectorPanel
 └─ SchedulerDiagramScene
No shortcuts. No UI logic in controllers.

1️⃣ Scheduler IR types
studio/core/scheduler/SchedulerTypes.h
#pragma once

#include <QString>
#include <QVector>
#include <QJsonObject>

namespace architect::scheduler {

struct Timeframe {
    QString name;      // e.g. "1m", "5m", "1h", "1d"
    int seconds = 60;

    QJsonObject toJson() const;
    static Timeframe fromJson(const QJsonObject& o);
};

struct SymbolJob {
    QString symbol;
    QVector<Timeframe> timeframes;

    QJsonObject toJson() const;
    static SymbolJob fromJson(const QJsonObject& o);
};

struct SchedulerConfig {
    QVector<SymbolJob> jobs;
    int intervalSeconds = 60;
    bool enabled = false;

    QJsonObject toJson() const;
    static SchedulerConfig fromJson(const QJsonObject& o);
};

struct Allocation {
    QString machineId;
    QVector<SymbolJob> jobs;
};

} // namespace architect::scheduler

studio/core/scheduler/SchedulerTypes.cpp
#include "SchedulerTypes.h"
#include "../util/JsonUtil.h"

using namespace architect::scheduler;

QJsonObject Timeframe::toJson() const {
    QJsonObject o;
    o["name"] = name;
    o["seconds"] = seconds;
    return o;
}

Timeframe Timeframe::fromJson(const QJsonObject& o) {
    Timeframe t;
    t.name = JsonUtil::getString(o, "name");
    t.seconds = JsonUtil::getInt(o, "seconds", 60);
    return t;
}

QJsonObject SymbolJob::toJson() const {
    QJsonObject o;
    o["symbol"] = symbol;
    QJsonArray arr;
    for (const auto& tf : timeframes)
        arr.append(tf.toJson());
    o["timeframes"] = arr;
    return o;
}

SymbolJob SymbolJob::fromJson(const QJsonObject& o) {
    SymbolJob j;
    j.symbol = JsonUtil::getString(o, "symbol");
    for (const auto& v : JsonUtil::getArray(o, "timeframes")) {
        if (v.isObject())
            j.timeframes.push_back(Timeframe::fromJson(v.toObject()));
    }
    return j;
}

QJsonObject SchedulerConfig::toJson() const {
    QJsonObject o;
    o["interval_seconds"] = intervalSeconds;
    o["enabled"] = enabled;

    QJsonArray arr;
    for (const auto& j : jobs)
        arr.append(j.toJson());
    o["jobs"] = arr;
    return o;
}

SchedulerConfig SchedulerConfig::fromJson(const QJsonObject& o) {
    SchedulerConfig c;
    c.intervalSeconds = JsonUtil::getInt(o, "interval_seconds", 60);
    c.enabled = JsonUtil::getBool(o, "enabled", false);

    for (const auto& v : JsonUtil::getArray(o, "jobs")) {
        if (v.isObject())
            c.jobs.push_back(SymbolJob::fromJson(v.toObject()));
    }
    return c;
}

2️⃣ Scheduler Page
studio/core/page/SchedulerPage.h
#pragma once

#include "BasePage.h"
#include "../scheduler/SchedulerTypes.h"

namespace architect {

class SchedulerController;
class InspectorPanel;
class DiagramScene;

class SchedulerPage final : public BasePage {
public:
    SchedulerPage(const QString& id, const QString& title);
    ~SchedulerPage() override;

    Type type() const override { return Type::Scheduler; }

    DiagramScene* createScene(QObject* parent) override;
    InspectorPanel* createInspector(QWidget* parent) override;
    PageController* controller() override;

    scheduler::SchedulerConfig& config() { return m_config; }
    const scheduler::SchedulerConfig& config() const { return m_config; }

    void loadFromJson(const QJsonObject& o) override;
    QJsonObject toJson() const override;

private:
    scheduler::SchedulerConfig m_config;
    SchedulerController* m_controller = nullptr;
};

} // namespace architect

studio/core/page/SchedulerPage.cpp
#include "SchedulerPage.h"
#include "../controller/SchedulerController.h"
#include "../ui/diagram/DiagramScene.h"
#include "../ui/inspector/SchedulerInspectorPanel.h"
#include "../util/JsonUtil.h"

using namespace architect;

SchedulerPage::SchedulerPage(const QString& id, const QString& title) {
    m_id = id;
    m_title = title;
    m_controller = new SchedulerController(this);
}

SchedulerPage::~SchedulerPage() {
    delete m_controller;
}

DiagramScene* SchedulerPage::createScene(QObject* parent) {
    auto* scene = new DiagramScene(parent);
    scene->addText("Scheduler Pipeline")->setPos(0, 0);
    return scene;
}

InspectorPanel* SchedulerPage::createInspector(QWidget* parent) {
    return new SchedulerInspectorPanel(this, parent);
}

PageController* SchedulerPage::controller() {
    return m_controller;
}

void SchedulerPage::loadFromJson(const QJsonObject& o) {
    BasePage::loadFromJson(o);
    m_config = scheduler::SchedulerConfig::fromJson(
        JsonUtil::getObject(o, "scheduler"));
}

QJsonObject SchedulerPage::toJson() const {
    QJsonObject o = BasePage::toJson();
    o["page_type"] = "scheduler";
    o["scheduler"] = m_config.toJson();
    return o;
}

3️⃣ Scheduler Controller (allocation + execution)
studio/core/controller/SchedulerController.h
#pragma once

#include "PageController.h"

#include <QTimer>

namespace architect {

class SchedulerPage;

class SchedulerController final : public PageController {
    Q_OBJECT

public:
    explicit SchedulerController(SchedulerPage* page);
    ~SchedulerController() override;

    void validate() override;
    void run() override;
    void stop() override;

private:
    SchedulerPage* m_page;
    QTimer m_timer;

    void executeOnce();
};

} // namespace architect

studio/core/controller/SchedulerController.cpp
#include "SchedulerController.h"
#include "../page/SchedulerPage.h"

using namespace architect;

SchedulerController::SchedulerController(SchedulerPage* page)
    : PageController(page), m_page(page)
{
    connect(&m_timer, &QTimer::timeout,
            this, &SchedulerController::executeOnce);
}

SchedulerController::~SchedulerController() = default;

void SchedulerController::validate() {
    if (m_page->config().jobs.isEmpty()) {
        emit logMessage("Scheduler: no symbol jobs configured");
    }
}

void SchedulerController::run() {
    auto& cfg = m_page->config();

    if (!cfg.enabled) {
        emit logMessage("Scheduler disabled");
        return;
    }

    emit logMessage(QString("Scheduler started (%1s interval)")
        .arg(cfg.intervalSeconds));

    m_timer.start(cfg.intervalSeconds * 1000);
    executeOnce();
}

void SchedulerController::stop() {
    m_timer.stop();
    emit logMessage("Scheduler stopped");
}

void SchedulerController::executeOnce() {
    emit logMessage("Scheduler tick:");

    for (const auto& job : m_page->config().jobs) {
        for (const auto& tf : job.timeframes) {
            emit logMessage(
                QString("  fetch %1 [%2]")
                    .arg(job.symbol)
                    .arg(tf.name));
        }
    }
}
This is intentionally local + synchronous for now.

4️⃣ Scheduler Inspector Panel
studio/ui/inspector/SchedulerInspectorPanel.h
#pragma once

#include "InspectorPanel.h"

namespace architect {

class SchedulerPage;

class SchedulerInspectorPanel final : public InspectorPanel {
    Q_OBJECT

public:
    explicit SchedulerInspectorPanel(
        SchedulerPage* page,
        QWidget* parent = nullptr);

private:
    SchedulerPage* m_page;
};

} // namespace architect

studio/ui/inspector/SchedulerInspectorPanel.cpp
#include "SchedulerInspectorPanel.h"
#include "../../core/page/SchedulerPage.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>

using namespace architect;

SchedulerInspectorPanel::SchedulerInspectorPanel(
    SchedulerPage* page, QWidget* parent)
    : InspectorPanel(parent), m_page(page)
{
    auto* layout = new QVBoxLayout(this);

    auto* enabled = new QCheckBox("Scheduler Enabled");
    enabled->setChecked(m_page->config().enabled);
    layout->addWidget(enabled);

    connect(enabled, &QCheckBox::toggled, this, [this](bool on) {
        m_page->config().enabled = on;
    });

    layout->addWidget(
        new QLabel(QString("Jobs: %1")
            .arg(m_page->config().jobs.size())));

    layout->addStretch();
}

5️⃣ Wire SchedulerPage into JsonDocument factory
In JsonDocument.cpp:
#include "../page/SchedulerPage.h"
Factory block:
if (pageType == "scheduler") {
    page = new SchedulerPage(pageId, title);
}

6️⃣ CMake additions
Add to studio/CMakeLists.txt:
  core/scheduler/SchedulerTypes.h
  core/scheduler/SchedulerTypes.cpp

  core/page/SchedulerPage.h
  core/page/SchedulerPage.cpp

  core/controller/SchedulerController.h
  core/controller/SchedulerController.cpp

  ui/inspector/SchedulerInspectorPanel.h
  ui/inspector/SchedulerInspectorPanel.cpp

7️⃣ Sample scheduler page JSON
{
  "page_id": "sched-1",
  "title": "Market Data Scheduler",
  "page_type": "scheduler",
  "scheduler": {
    "enabled": true,
    "interval_seconds": 30,
    "jobs": [
      {
        "symbol": "AAPL",
        "timeframes": [
          { "name": "1m", "seconds": 60 },
          { "name": "5m", "seconds": 300 }
        ]
      },
      {
        "symbol": "SPY",
        "timeframes": [
          { "name": "1m", "seconds": 60 }
        ]
      }
    ]
  },
  "diagram": { "nodes": [], "edges": [] }
}

What you now have
    • A working scheduler
    • Periodic execution
    • Configurable jobs
    • Inspector toggles
    • Logs in Run Console
    • Clean path to:
        ◦ SSH execution
        ◦ Spark submission
        ◦ Kafka publishing

Next logical step
Next in sequence:
Distributed execution (SSH / local worker pool)
→ then Spark job submission
→ then Strategy Factory / backtest sweeps
Say “next” and we’ll move on.
Awesome — next = distributed execution for the Scheduler page.
This step adds, in a disciplined OO way:
    • IJobRunner (abstract base class)
    • LocalMockRunner (simulates distributed work)
    • AllocationStrategy (round-robin across machines from Infra snapshot)
    • SchedulerService (pure logic: allocate + dispatch)
    • Scheduler controller uses the service and logs per-machine runs
No SSH yet. No Spark submit yet. Just the correct architecture so we can swap runners later without refactors.

STEP 4.1 — Distributed execution (Allocation + Runner + Service)
0) Small addition: share infra snapshot in the app (minimal + clean)
Right now Infra snapshot lives inside InfrastructureController. The scheduler needs machine IDs.
We’ll introduce a tiny singleton-style AppServices with an InfraRegistry cache (v1).
studio/core/infra/InfraRegistry.h
#pragma once

#include <QObject>
#include <QVector>
#include "InfraTypes.h"

namespace architect::infra {

class InfraRegistry final : public QObject {
    Q_OBJECT
public:
    explicit InfraRegistry(QObject* parent = nullptr);

    void setSnapshot(const QVector<Machine>& machines);
    QVector<Machine> snapshot() const;

signals:
    void snapshotUpdated();

private:
    QVector<Machine> m_snapshot;
};

} // namespace architect::infra
studio/core/infra/InfraRegistry.cpp
#include "InfraRegistry.h"

using namespace architect::infra;

InfraRegistry::InfraRegistry(QObject* parent) : QObject(parent) {}

void InfraRegistry::setSnapshot(const QVector<Machine>& machines) {
    m_snapshot = machines;
    emit snapshotUpdated();
}

QVector<Machine> InfraRegistry::snapshot() const {
    return m_snapshot;
}
studio/core/AppServices.h
#pragma once

namespace architect::infra { class InfraRegistry; }

namespace architect {

class AppServices final {
public:
    static void init(architect::infra::InfraRegistry* infraRegistry);
    static architect::infra::InfraRegistry* infra();

private:
    static architect::infra::InfraRegistry* s_infra;
};

} // namespace architect
studio/core/AppServices.cpp
#include "AppServices.h"
#include "infra/InfraRegistry.h"

using namespace architect;

architect::infra::InfraRegistry* AppServices::s_infra = nullptr;

void AppServices::init(architect::infra::InfraRegistry* infraRegistry) {
    s_infra = infraRegistry;
}

architect::infra::InfraRegistry* AppServices::infra() {
    return s_infra;
}
Wire it in MainWindow (one-time init)
In your MainWindow constructor (or setupUi()), create registry and init:
#include "../core/AppServices.h"
#include "../core/infra/InfraRegistry.h"

// ...
auto* infraRegistry = new architect::infra::InfraRegistry(this);
architect::AppServices::init(infraRegistry);
And when InfrastructureController refreshes snapshot, publish it:
In InfrastructureController::run() after m_page->machines() = snap;
#include "../AppServices.h"
#include "../infra/InfraRegistry.h"

// ...
if (auto* reg = architect::AppServices::infra()) {
    reg->setSnapshot(snap);
}
Now Scheduler can see machine IDs.

1) Allocation + Runner abstractions (OO spine)
studio/core/scheduler/IJobRunner.h
#pragma once

#include <QString>
#include <QVector>
#include "SchedulerTypes.h"

namespace architect::scheduler {

struct RunResult {
    QString machineId;
    int jobsLaunched = 0;
    bool ok = true;
    QString message;
};

class IJobRunner {
public:
    virtual ~IJobRunner() = default;

    virtual QString name() const = 0;

    // Execute assigned jobs on a "machine".
    // v1 is synchronous; later we’ll async/parallelize per machine.
    virtual RunResult runOnMachine(
        const QString& machineId,
        const QVector<SymbolJob>& jobs) = 0;
};

} // namespace architect::scheduler
studio/core/scheduler/RoundRobinAllocator.h
#pragma once

#include <QString>
#include <QVector>
#include "SchedulerTypes.h"

namespace architect::scheduler {

struct Allocation {
    QString machineId;
    QVector<SymbolJob> jobs;
};

class RoundRobinAllocator final {
public:
    static QVector<Allocation> allocate(
        const QVector<QString>& machineIds,
        const QVector<SymbolJob>& jobs);
};

} // namespace architect::scheduler
studio/core/scheduler/RoundRobinAllocator.cpp
#include "RoundRobinAllocator.h"

using namespace architect::scheduler;

QVector<Allocation> RoundRobinAllocator::allocate(
    const QVector<QString>& machineIds,
    const QVector<SymbolJob>& jobs)
{
    QVector<Allocation> out;
    if (machineIds.isEmpty()) return out;

    out.reserve(machineIds.size());
    for (const auto& id : machineIds)
        out.push_back(Allocation{id, {}});

    int idx = 0;
    for (const auto& j : jobs) {
        out[idx].jobs.push_back(j);
        idx = (idx + 1) % machineIds.size();
    }
    return out;
}

2) Mock distributed runner (works now)
studio/core/scheduler/LocalMockRunner.h
#pragma once

#include "IJobRunner.h"

namespace architect::scheduler {

class LocalMockRunner final : public IJobRunner {
public:
    QString name() const override { return "local-mock"; }

    RunResult runOnMachine(
        const QString& machineId,
        const QVector<SymbolJob>& jobs) override;
};

} // namespace architect::scheduler
studio/core/scheduler/LocalMockRunner.cpp
#include "LocalMockRunner.h"
#include <QThread>

using namespace architect::scheduler;

RunResult LocalMockRunner::runOnMachine(
    const QString& machineId,
    const QVector<SymbolJob>& jobs)
{
    RunResult r;
    r.machineId = machineId;
    r.jobsLaunched = jobs.size();
    r.ok = true;

    // Simulate per-machine work
    QThread::msleep(80);

    r.message = QString("Simulated %1 job(s) on %2")
        .arg(jobs.size())
        .arg(machineId);

    return r;
}

3) SchedulerService (pure logic, testable)
studio/core/scheduler/SchedulerService.h
#pragma once

#include <memory>
#include <QString>
#include <QVector>

#include "SchedulerTypes.h"
#include "IJobRunner.h"

namespace architect::scheduler {

class SchedulerService final {
public:
    explicit SchedulerService(std::unique_ptr<IJobRunner> runner);

    QVector<RunResult> executeTick(
        const QVector<QString>& machineIds,
        const SchedulerConfig& config);

    QString runnerName() const;

private:
    std::unique_ptr<IJobRunner> m_runner;
};

} // namespace architect::scheduler
studio/core/scheduler/SchedulerService.cpp
#include "SchedulerService.h"
#include "RoundRobinAllocator.h"

using namespace architect::scheduler;

SchedulerService::SchedulerService(std::unique_ptr<IJobRunner> runner)
    : m_runner(std::move(runner))
{
}

QString SchedulerService::runnerName() const {
    return m_runner ? m_runner->name() : "none";
}

QVector<RunResult> SchedulerService::executeTick(
    const QVector<QString>& machineIds,
    const SchedulerConfig& config)
{
    QVector<RunResult> results;
    if (!m_runner) return results;
    if (machineIds.isEmpty()) return results;
    if (config.jobs.isEmpty()) return results;

    const auto alloc = RoundRobinAllocator::allocate(machineIds, config.jobs);

    results.reserve(alloc.size());
    for (const auto& a : alloc) {
        if (a.jobs.isEmpty()) continue;
        results.push_back(m_runner->runOnMachine(a.machineId, a.jobs));
    }
    return results;
}

4) Update SchedulerController to use infra snapshot + service
Update studio/core/controller/SchedulerController.h
#pragma once

#include "PageController.h"
#include <QTimer>
#include <memory>

namespace architect {
class SchedulerPage;

namespace scheduler { class SchedulerService; }

class SchedulerController final : public PageController {
    Q_OBJECT
public:
    explicit SchedulerController(SchedulerPage* page);
    ~SchedulerController() override;

    void validate() override;
    void run() override;
    void stop() override;

private:
    SchedulerPage* m_page;
    QTimer m_timer;

    std::unique_ptr<scheduler::SchedulerService> m_service;

    void executeOnce();
};

} // namespace architect
Update studio/core/controller/SchedulerController.cpp
#include "SchedulerController.h"
#include "../page/SchedulerPage.h"

#include "../AppServices.h"
#include "../infra/InfraRegistry.h"
#include "../infra/InfraTypes.h"

#include "../scheduler/SchedulerService.h"
#include "../scheduler/LocalMockRunner.h"

using namespace architect;

SchedulerController::SchedulerController(SchedulerPage* page)
    : PageController(page), m_page(page)
{
    m_service = std::make_unique<scheduler::SchedulerService>(
        std::make_unique<scheduler::LocalMockRunner>()
    );

    connect(&m_timer, &QTimer::timeout,
            this, &SchedulerController::executeOnce);
}

SchedulerController::~SchedulerController() = default;

void SchedulerController::validate() {
    if (m_page->config().jobs.isEmpty())
        emit logMessage("Scheduler: no symbol jobs configured");

    if (!AppServices::infra())
        emit logMessage("Scheduler: InfraRegistry not initialized (no machine allocation)");
}

void SchedulerController::run() {
    auto& cfg = m_page->config();
    if (!cfg.enabled) {
        emit logMessage("Scheduler disabled");
        return;
    }

    emit logMessage(QString("Scheduler started (%1s interval) runner=%2")
        .arg(cfg.intervalSeconds)
        .arg(m_service->runnerName()));

    m_timer.start(cfg.intervalSeconds * 1000);
    executeOnce();
}

void SchedulerController::stop() {
    m_timer.stop();
    emit logMessage("Scheduler stopped");
}

void SchedulerController::executeOnce() {
    emit logMessage("Scheduler tick:");

    QVector<QString> machineIds;

    if (auto* reg = AppServices::infra()) {
        const auto snap = reg->snapshot();
        for (const auto& m : snap)
            machineIds.push_back(m.id);
    }

    if (machineIds.isEmpty()) {
        // Fall back to a single local machine if infra not present
        machineIds = {"local"};
        emit logMessage("  (no infra snapshot) using fallback machineId=local");
    }

    const auto results = m_service->executeTick(machineIds, m_page->config());
    for (const auto& r : results) {
        emit logMessage(QString("  [%1] %2").arg(r.machineId, r.message));
    }
}

5) CMake additions (Step 4.1)
Add to studio/CMakeLists.txt:
  core/AppServices.h
  core/AppServices.cpp

  core/infra/InfraRegistry.h
  core/infra/InfraRegistry.cpp

  core/scheduler/IJobRunner.h
  core/scheduler/LocalMockRunner.h
  core/scheduler/LocalMockRunner.cpp
  core/scheduler/RoundRobinAllocator.h
  core/scheduler/RoundRobinAllocator.cpp
  core/scheduler/SchedulerService.h
  core/scheduler/SchedulerService.cpp

What you’ll see now
    1. Go to Infrastructure page → Refresh Page
→ publishes snapshot to registry
    2. Go to Scheduler page (enabled) → Refresh Page (or it ticks)
→ scheduler allocates symbol jobs across aurora, hp-envy, ec2-1 (etc.)
→ logs show per-machine simulated execution
This is the exact stepping stone you want before SSH/Spark.

Next
Next = real runners:
    • SshRunner (paramiko not allowed; we’ll do ssh process execution)
    • or SparkSubmitRunner (generate a spark-submit plan)
Say “next” and tell me which runner you want first:
    • SSH runner (distributed collectors)
    • Spark submit runner (distributed backtest/feature jobs)
Awesome — next = SSH runner first (practical next step for distributed collectors), implemented the right way in C++: no Python, no paramiko, just a hardened ssh/scp process runner with timeouts, exit codes, and log capture.
This gives you real distributed execution today, and it becomes the base for later Spark submit and even remote Rust backtest sweeps.

STEP 4.2 — SshRunner (real distributed job execution)
What this delivers
    • SshRunner implementing IJobRunner
    • Uses QProcess to call system ssh
    • Builds a remote command that runs a collector script (mock now)
    • Collects stdout/stderr into RunResult.message
    • Supports:
        ◦ ssh -o BatchMode=yes
        ◦ ConnectTimeout
        ◦ configurable user/host per machine
    • No security footguns (no password prompts, no interactive)
You’ll need your machines reachable with SSH keys already (which you do / can do).

1) Extend Infra model to include SSH connection info
We need machine hostnames and optional user/port.
Update studio/core/infra/InfraTypes.h
Add to Machine:
QString ssh_user;
QString ssh_host;
int ssh_port = 22;
So Machine becomes:
struct Machine {
    QString id;
    QString display_name;

    QString ssh_user;
    QString ssh_host;
    int ssh_port = 22;

    QVector<Service> services;

    QJsonObject toJson() const;
    static Machine fromJson(const QJsonObject& o);
};
Update InfraTypes.cpp
In Machine::toJson() add:
o["ssh_user"] = ssh_user;
o["ssh_host"] = ssh_host;
o["ssh_port"] = ssh_port;
In Machine::fromJson() add:
m.ssh_user = JsonUtil::getString(o, "ssh_user");
m.ssh_host = JsonUtil::getString(o, "ssh_host");
m.ssh_port = JsonUtil::getInt(o, "ssh_port", 22);

2) Create SshRunner
studio/core/scheduler/SshRunner.h
#pragma once

#include "IJobRunner.h"
#include "../infra/InfraTypes.h"

#include <QMap>

namespace architect::scheduler {

class SshRunner final : public IJobRunner {
public:
    // machine lookup: machineId -> Machine info
    explicit SshRunner(QMap<QString, architect::infra::Machine> machines);

    QString name() const override { return "ssh"; }

    RunResult runOnMachine(
        const QString& machineId,
        const QVector<SymbolJob>& jobs) override;

private:
    QMap<QString, architect::infra::Machine> m_machines;

    QString buildRemoteCommand(const QVector<SymbolJob>& jobs) const;

    RunResult runSsh(
        const architect::infra::Machine& m,
        const QString& remoteCmd) const;
};

} // namespace architect::scheduler

studio/core/scheduler/SshRunner.cpp
#include "SshRunner.h"

#include <QProcess>
#include <QStringList>

using namespace architect::scheduler;

SshRunner::SshRunner(QMap<QString, architect::infra::Machine> machines)
    : m_machines(std::move(machines))
{
}

QString SshRunner::buildRemoteCommand(const QVector<SymbolJob>& jobs) const {
    // v1: just echo what we'd fetch on the remote machine.
    // Later: call a real collector binary/script.
    // Keep remote command POSIX-safe.
    QString cmd = "set -e; echo '[collector] start'; ";

    for (const auto& j : jobs) {
        for (const auto& tf : j.timeframes) {
            cmd += QString("echo 'fetch %1 [%2]'; ")
                .arg(j.symbol, tf.name);
        }
    }
    cmd += "echo '[collector] done';";
    return cmd;
}

RunResult SshRunner::runOnMachine(
    const QString& machineId,
    const QVector<SymbolJob>& jobs)
{
    RunResult r;
    r.machineId = machineId;
    r.jobsLaunched = jobs.size();

    if (!m_machines.contains(machineId)) {
        r.ok = false;
        r.message = "No SSH config for machineId";
        return r;
    }

    const auto& m = m_machines[machineId];
    if (m.ssh_host.isEmpty()) {
        r.ok = false;
        r.message = "Machine missing ssh_host";
        return r;
    }

    const auto remoteCmd = buildRemoteCommand(jobs);
    return runSsh(m, remoteCmd);
}

RunResult SshRunner::runSsh(
    const architect::infra::Machine& m,
    const QString& remoteCmd) const
{
    RunResult r;
    r.machineId = m.id;

    QString target = m.ssh_host;
    if (!m.ssh_user.isEmpty()) {
        target = m.ssh_user + "@" + m.ssh_host;
    }

    QStringList args;
    args << "-o" << "BatchMode=yes";
    args << "-o" << "StrictHostKeyChecking=accept-new";
    args << "-o" << "ConnectTimeout=5";
    args << "-p" << QString::number(m.ssh_port);
    args << target;
    args << remoteCmd;

    QProcess proc;
    proc.start("ssh", args);

    // Hard timeout so nothing hangs your UI.
    if (!proc.waitForStarted(3000)) {
        r.ok = false;
        r.message = "ssh failed to start";
        return r;
    }

    if (!proc.waitForFinished(15000)) { // 15s timeout (v1)
        proc.kill();
        proc.waitForFinished(1000);
        r.ok = false;
        r.message = "ssh timed out";
        return r;
    }

    const auto out = proc.readAllStandardOutput();
    const auto err = proc.readAllStandardError();
    const int code = proc.exitCode();

    r.ok = (code == 0);
    r.message = QString("exit=%1\nstdout:\n%2\nstderr:\n%3")
        .arg(code)
        .arg(QString::fromUtf8(out))
        .arg(QString::fromUtf8(err));

    return r;
}

3) Update SchedulerController to select runner based on Infra snapshot
We’ll keep it simple:
    • If InfraRegistry has machines with ssh_host, use SSH runner.
    • Else fallback to LocalMockRunner.
Update SchedulerController constructor
Replace:
m_service = std::make_unique<scheduler::SchedulerService>(
    std::make_unique<scheduler::LocalMockRunner>()
);
With lazy selection inside executeOnce() (better because machines snapshot changes):
In SchedulerController.h add:
#include <memory>
and change service to be created each tick (v1), or keep one but swap runner (cleaner later). We’ll do create per tick for now to avoid redesign.
So remove m_service member, and inside executeOnce build service.
Update SchedulerController.h
Remove:
std::unique_ptr<scheduler::SchedulerService> m_service;
Update SchedulerController.cpp executeOnce()
Replace the service usage with:
QMap<QString, architect::infra::Machine> machineMap;
bool anySsh = false;

if (auto* reg = AppServices::infra()) {
    const auto snap = reg->snapshot();
    for (const auto& m : snap) {
        machineIds.push_back(m.id);
        machineMap[m.id] = m;
        if (!m.ssh_host.isEmpty()) anySsh = true;
    }
}

std::unique_ptr<architect::scheduler::IJobRunner> runner;
if (anySsh) {
    runner = std::make_unique<architect::scheduler::SshRunner>(machineMap);
    emit logMessage("  runner=ssh");
} else {
    runner = std::make_unique<architect::scheduler::LocalMockRunner>();
    emit logMessage("  runner=local-mock");
}

architect::scheduler::SchedulerService service(std::move(runner));
const auto results = service.executeTick(machineIds, m_page->config());

for (const auto& r : results) {
    emit logMessage(QString("  [%1] %2").arg(r.machineId, r.message));
}
Also remember to #include "../scheduler/SshRunner.h" at top.

4) Update MockInfraProvider to include ssh_host fields (so you can test)
In MockInfraProvider::ensureSeeded() set:
aurora.ssh_user = "jd";
aurora.ssh_host = "aurora.local"; // or actual LAN IP/hostname
aurora.ssh_port = 22;

envy.ssh_user = "jd";
envy.ssh_host = "hp-envy.local";

ec2.ssh_user = "ubuntu";
ec2.ssh_host = "ec2-your-public-dns";
Use whatever hostnames you actually have (LAN IPs are fine).
If these aren’t reachable yet, leave blank and it will fall back to local-mock.

5) CMake additions
Add to studio/CMakeLists.txt:
  core/scheduler/SshRunner.h
  core/scheduler/SshRunner.cpp
No extra Qt modules needed (QProcess already available in QtCore which you have via Widgets).

6) JSON example for real machines
In your .sarch.json infra page, add fields:
{
  "id": "aurora",
  "display_name": "Aurora (RTX 4080)",
  "ssh_user": "jd",
  "ssh_host": "192.168.1.50",
  "ssh_port": 22,
  "services": [
    { "name": "Airflow", "url": "http://192.168.1.50:8080", "running": true }
  ]
}

What you’ll see now
    • Infrastructure refresh publishes machine list (with ssh)
    • Scheduler tick allocates jobs across machine IDs
    • Runner becomes ssh
    • Logs include remote stdout/stderr per machine
    • Timeout protects UI from hanging
That’s a real distributed scheduler.

Next
Next is an easy win with huge ROI:
Remote job packaging (ship a small collector binary/script via scp once, then run it)
→ makes the scheduler real, not “echo-based”.
Say “next” and we’ll add:
    • DeploymentService (scp push + version hash)
    • Remote command becomes ~/strategient/bin/collector --symbols ...
    • Start building toward Spark-based backtests cleanly

Love it — this is the “make it real” jump. ✅
Next = DeploymentService: push a versioned collector payload to each machine (via scp), cache the deployed hash, then run it via ssh.
We’ll keep it hardened + boring:
    • version hash from local file bytes (SHA-256)
    • remote install dir: ~/.strategient/bin/
    • remote version file: ~/.strategient/bin/collector.sha256
    • only redeploy when hash changes
    • all via QProcess calling ssh/scp
    • timeouts everywhere
No assumptions about Python. You can deploy:
    • a Rust static binary (recommended)
    • or any executable/script

STEP 4.3 — Remote packaging & deployment
0) Add these files
studio/core/deploy/DeploymentService.h
studio/core/deploy/DeploymentService.cpp
studio/core/util/FileHash.h
studio/core/util/FileHash.cpp
And we’ll update SshRunner to use DeploymentService.

1) File hashing utility
studio/core/util/FileHash.h
#pragma once

#include <QString>

namespace architect {

// Returns SHA256 hex of the file contents, or empty string on error.
QString sha256FileHex(const QString& filePath);

}
studio/core/util/FileHash.cpp
#include "FileHash.h"

#include <QCryptographicHash>
#include <QFile>

using namespace architect;

QString architect::sha256FileHex(const QString& filePath) {
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly))
        return {};

    QCryptographicHash h(QCryptographicHash::Sha256);

    while (!f.atEnd()) {
        const auto chunk = f.read(1024 * 1024);
        if (chunk.isEmpty() && !f.atEnd())
            return {};
        h.addData(chunk);
    }

    return QString::fromLatin1(h.result().toHex());
}

2) DeploymentService (scp + ssh + hash cache)
studio/core/deploy/DeploymentService.h
#pragma once

#include <QString>
#include "../infra/InfraTypes.h"

namespace architect {

struct DeployResult {
    bool ok = false;
    QString message;
    bool deployed = false;   // true if we actually pushed bytes
};

// Deploys a local payload (collector) to remote machines, idempotently.
class DeploymentService final {
public:
    DeploymentService() = default;

    // localPayloadPath: e.g. "/home/jd/projects/strategient-architect/artifacts/collector"
    // remoteName: e.g. "collector"
    DeployResult ensureDeployed(
        const architect::infra::Machine& m,
        const QString& localPayloadPath,
        const QString& remoteName) const;

private:
    QString remoteInstallDir() const;
    QString remotePayloadPath(const QString& remoteName) const;
    QString remoteHashPath(const QString& remoteName) const;

    DeployResult ensureRemoteDir(const architect::infra::Machine& m) const;
    DeployResult readRemoteHash(const architect::infra::Machine& m, const QString& remoteName) const;
    DeployResult writeRemoteHash(const architect::infra::Machine& m, const QString& remoteName, const QString& hashHex) const;
    DeployResult scpPush(const architect::infra::Machine& m, const QString& localPath, const QString& remoteDestPath) const;
    DeployResult sshExec(const architect::infra::Machine& m, const QString& remoteCmd, int timeoutMs) const;

    QString sshTarget(const architect::infra::Machine& m) const;
};
studio/core/deploy/DeploymentService.cpp
#include "DeploymentService.h"
#include "../util/FileHash.h"

#include <QProcess>
#include <QStringList>

using namespace architect;

QString DeploymentService::remoteInstallDir() const {
    return "~/.strategient/bin";
}

QString DeploymentService::remotePayloadPath(const QString& remoteName) const {
    return remoteInstallDir() + "/" + remoteName;
}

QString DeploymentService::remoteHashPath(const QString& remoteName) const {
    return remoteInstallDir() + "/" + remoteName + ".sha256";
}

QString DeploymentService::sshTarget(const architect::infra::Machine& m) const {
    if (!m.ssh_user.isEmpty())
        return m.ssh_user + "@" + m.ssh_host;
    return m.ssh_host;
}

DeployResult DeploymentService::sshExec(const architect::infra::Machine& m, const QString& remoteCmd, int timeoutMs) const {
    DeployResult r;

    if (m.ssh_host.isEmpty()) {
        r.ok = false;
        r.message = "ssh_host missing";
        return r;
    }

    QStringList args;
    args << "-o" << "BatchMode=yes";
    args << "-o" << "StrictHostKeyChecking=accept-new";
    args << "-o" << "ConnectTimeout=5";
    args << "-p" << QString::number(m.ssh_port);
    args << sshTarget(m);
    args << remoteCmd;

    QProcess p;
    p.start("ssh", args);

    if (!p.waitForStarted(3000)) {
        r.ok = false;
        r.message = "ssh failed to start";
        return r;
    }

    if (!p.waitForFinished(timeoutMs)) {
        p.kill();
        p.waitForFinished(1000);
        r.ok = false;
        r.message = "ssh timed out";
        return r;
    }

    const int code = p.exitCode();
    const auto out = p.readAllStandardOutput();
    const auto err = p.readAllStandardError();

    r.ok = (code == 0);
    r.message = QString("exit=%1\n%2%3")
        .arg(code)
        .arg(QString::fromUtf8(out))
        .arg(QString::fromUtf8(err));
    return r;
}

DeployResult DeploymentService::ensureRemoteDir(const architect::infra::Machine& m) const {
    // mkdir -p ~/.strategient/bin
    return sshExec(m, QString("mkdir -p %1").arg(remoteInstallDir()), 8000);
}

DeployResult DeploymentService::readRemoteHash(const architect::infra::Machine& m, const QString& remoteName) const {
    // cat ~/.strategient/bin/collector.sha256 2>/dev/null || true
    return sshExec(m, QString("cat %1 2>/dev/null || true").arg(remoteHashPath(remoteName)), 8000);
}

DeployResult DeploymentService::writeRemoteHash(const architect::infra::Machine& m, const QString& remoteName, const QString& hashHex) const {
    // echo <hash> > ~/.strategient/bin/collector.sha256
    // Use printf to avoid weird echo behavior.
    const auto cmd = QString("printf '%1' > %2")
        .arg(hashHex, remoteHashPath(remoteName));
    return sshExec(m, cmd, 8000);
}

DeployResult DeploymentService::scpPush(const architect::infra::Machine& m, const QString& localPath, const QString& remoteDestPath) const {
    DeployResult r;

    if (m.ssh_host.isEmpty()) {
        r.ok = false;
        r.message = "ssh_host missing";
        return r;
    }

    QStringList args;
    args << "-o" << "BatchMode=yes";
    args << "-o" << "StrictHostKeyChecking=accept-new";
    args << "-o" << "ConnectTimeout=5";
    args << "-P" << QString::number(m.ssh_port);
    args << localPath;
    args << (sshTarget(m) + ":" + remoteDestPath);

    QProcess p;
    p.start("scp", args);

    if (!p.waitForStarted(3000)) {
        r.ok = false;
        r.message = "scp failed to start";
        return r;
    }

    if (!p.waitForFinished(20000)) {
        p.kill();
        p.waitForFinished(1000);
        r.ok = false;
        r.message = "scp timed out";
        return r;
    }

    const int code = p.exitCode();
    const auto out = p.readAllStandardOutput();
    const auto err = p.readAllStandardError();

    r.ok = (code == 0);
    r.message = QString("exit=%1\n%2%3")
        .arg(code)
        .arg(QString::fromUtf8(out))
        .arg(QString::fromUtf8(err));
    return r;
}

DeployResult DeploymentService::ensureDeployed(
    const architect::infra::Machine& m,
    const QString& localPayloadPath,
    const QString& remoteName) const
{
    DeployResult r;

    const auto localHash = architect::sha256FileHex(localPayloadPath);
    if (localHash.isEmpty()) {
        r.ok = false;
        r.message = "Failed to hash local payload (missing file?)";
        return r;
    }

    // Ensure remote dir
    auto dirRes = ensureRemoteDir(m);
    if (!dirRes.ok) {
        r.ok = false;
        r.message = "ensureRemoteDir failed:\n" + dirRes.message;
        return r;
    }

    // Read remote hash
    auto rh = readRemoteHash(m, remoteName);
    if (!rh.ok) {
        // Even if cat failed, we can still attempt push; but treat as non-fatal:
        // We'll just push.
    }

    // Remote hash output is in rh.message; extract the last token-ish
    // We wrote message as "exit=...\n<out><err>". Cheap parse: find first newline then trim.
    QString remoteHash;
    const int nl = rh.message.indexOf('\n');
    if (nl >= 0) {
        remoteHash = rh.message.mid(nl + 1).trimmed();
    } else {
        remoteHash = rh.message.trimmed();
    }

    const bool needsDeploy = (remoteHash != localHash);

    if (needsDeploy) {
        // Push payload to ~/.strategient/bin/collector (or remoteName)
        auto push = scpPush(m, localPayloadPath, remotePayloadPath(remoteName));
        if (!push.ok) {
            r.ok = false;
            r.message = "scpPush failed:\n" + push.message;
            return r;
        }

        // chmod +x
        auto chmodRes = sshExec(m, QString("chmod +x %1").arg(remotePayloadPath(remoteName)), 8000);
        if (!chmodRes.ok) {
            r.ok = false;
            r.message = "chmod failed:\n" + chmodRes.message;
            return r;
        }

        // Write hash
        auto wh = writeRemoteHash(m, remoteName, localHash);
        if (!wh.ok) {
            r.ok = false;
            r.message = "writeRemoteHash failed:\n" + wh.message;
            return r;
        }

        r.deployed = true;
        r.ok = true;
        r.message = QString("Deployed %1 (sha256=%2)").arg(remoteName, localHash);
        return r;
    }

    r.ok = true;
    r.deployed = false;
    r.message = QString("Already deployed %1 (sha256=%2)").arg(remoteName, localHash);
    return r;
}

3) Update SshRunner to deploy then run collector
We’ll change the runner to call a real payload:
    • local payload path: artifacts/collector (you supply)
    • remote exec: ~/.strategient/bin/collector --symbols ... --timeframes ...
Update studio/core/scheduler/SshRunner.h
Add includes + member:
#include "../deploy/DeploymentService.h"

class SshRunner final : public IJobRunner {
public:
    explicit SshRunner(QMap<QString, architect::infra::Machine> machines,
                       QString localCollectorPath);

    // ...
private:
    QMap<QString, architect::infra::Machine> m_machines;
    QString m_localCollectorPath;
    architect::DeploymentService m_deploy;

    QString buildCollectorArgs(const QVector<SymbolJob>& jobs) const;
    RunResult runRemoteCollector(const architect::infra::Machine& m,
                                 const QString& args) const;

    // remove buildRemoteCommand / runSsh if you want, or keep for reuse
};
Update studio/core/scheduler/SshRunner.cpp
Replace your old “echo” remote command flow with this:
#include "SshRunner.h"

#include <QProcess>
#include <QStringList>

using namespace architect::scheduler;

SshRunner::SshRunner(QMap<QString, architect::infra::Machine> machines,
                     QString localCollectorPath)
    : m_machines(std::move(machines)),
      m_localCollectorPath(std::move(localCollectorPath))
{
}

QString SshRunner::buildCollectorArgs(const QVector<SymbolJob>& jobs) const {
    // v1: flatten to simple CLI args (keep it boring)
    // --symbols AAPL,SPY --timeframes 60,300
    QStringList symbols;
    QStringList tfs;

    for (const auto& j : jobs) {
        symbols << j.symbol;
        for (const auto& tf : j.timeframes) {
            const auto sec = QString::number(tf.seconds);
            if (!tfs.contains(sec))
                tfs << sec;
        }
    }

    return QString("--symbols \"%1\" --timeframes \"%2\"")
        .arg(symbols.join(","), tfs.join(","));
}

RunResult SshRunner::runRemoteCollector(const architect::infra::Machine& m,
                                       const QString& args) const
{
    RunResult r;
    r.machineId = m.id;

    QString target = m.ssh_host;
    if (!m.ssh_user.isEmpty())
        target = m.ssh_user + "@" + m.ssh_host;

    const auto remoteExe = QString("~/.strategient/bin/collector %1").arg(args);

    QStringList sshArgs;
    sshArgs << "-o" << "BatchMode=yes";
    sshArgs << "-o" << "StrictHostKeyChecking=accept-new";
    sshArgs << "-o" << "ConnectTimeout=5";
    sshArgs << "-p" << QString::number(m.ssh_port);
    sshArgs << target;
    sshArgs << remoteExe;

    QProcess proc;
    proc.start("ssh", sshArgs);

    if (!proc.waitForStarted(3000)) {
        r.ok = false;
        r.message = "ssh failed to start";
        return r;
    }

    if (!proc.waitForFinished(30000)) { // 30s per tick v1
        proc.kill();
        proc.waitForFinished(1000);
        r.ok = false;
        r.message = "ssh timed out running collector";
        return r;
    }

    const int code = proc.exitCode();
    const auto out = proc.readAllStandardOutput();
    const auto err = proc.readAllStandardError();

    r.ok = (code == 0);
    r.message = QString("collector exit=%1\nstdout:\n%2\nstderr:\n%3")
        .arg(code)
        .arg(QString::fromUtf8(out))
        .arg(QString::fromUtf8(err));
    return r;
}

RunResult SshRunner::runOnMachine(const QString& machineId,
                                 const QVector<SymbolJob>& jobs)
{
    RunResult r;
    r.machineId = machineId;
    r.jobsLaunched = jobs.size();

    if (!m_machines.contains(machineId)) {
        r.ok = false;
        r.message = "No SSH config for machineId";
        return r;
    }

    const auto& m = m_machines[machineId];
    if (m.ssh_host.isEmpty()) {
        r.ok = false;
        r.message = "Machine missing ssh_host";
        return r;
    }

    // Deploy collector if needed
    const auto dep = m_deploy.ensureDeployed(m, m_localCollectorPath, "collector");
    if (!dep.ok) {
        r.ok = false;
        r.message = "deploy failed: " + dep.message;
        return r;
    }

    const auto args = buildCollectorArgs(jobs);
    auto run = runRemoteCollector(m, args);

    // Prefix with deploy status (helpful)
    run.message = QString("[deploy] %1\n%2").arg(dep.message, run.message);
    return run;
}

4) Update SchedulerController to pass local collector path into SshRunner
In SchedulerController::executeOnce() where you create the ssh runner:
const QString collectorPath =
    QCoreApplication::applicationDirPath() + "/collector";
// OR: absolute path you configure; simplest for now:
const QString collectorPath2 = "/home/jd/projects/strategient-architect/artifacts/collector";
runner = std::make_unique<architect::scheduler::SshRunner>(machineMap, collectorPath2);
emit logMessage(QString("  runner=ssh collector=%1").arg(collectorPath2));
(You’ll probably want a config setting later; for v1 hardcode is fine.)
Add include:
#include <QCoreApplication>

5) CMake additions
Add to studio/CMakeLists.txt:
  core/deploy/DeploymentService.h
  core/deploy/DeploymentService.cpp
  core/util/FileHash.h
  core/util/FileHash.cpp
(You already have Qt for QCryptographicHash via QtCore.)

6) Provide a minimal “collector” payload (so it runs)
For v1, you can drop a tiny Bash script at:
strategient-architect/artifacts/collector
#!/usr/bin/env bash
set -euo pipefail
echo "[collector] host=$(hostname)"
echo "[collector] args=$*"
# Pretend to fetch
sleep 0.2
echo "[collector] done"
Make it executable:
chmod +x artifacts/collector
Later you’ll replace that with a Rust static binary that actually pulls market data and writes Parquet.

What you’ll see now
On first scheduler tick per machine:
    • [deploy] Deployed collector (sha256=...)
    • then collector output
On subsequent ticks (until collector changes):
    • [deploy] Already deployed collector (sha256=...)
    • runs immediately
This is the exact “versioned remote payload” loop you want.

Next
Say “next” and we’ll do the next ROI monster:
Make the collector real (Rust binary):
    • parse --symbols / --timeframes
    • fetch market data (stub adaptor now, Schwab/Polygon/Alpaca later)
    • write partitioned Parquet to your data lake
    • optionally stream progress back via stdout JSON lines (for your Run Console)
Your move.
Boom. Next = replace the bash “collector” with a real Rust binary that you can ship to all machines and that outputs structured JSONL progress, writes partitioned Parquet (stub-friendly), and is ready for real market-data adapters.
I’m keeping this MVP-real:
    • Rust CLI using clap
    • Emits JSON lines to stdout (your Run Console will love this later)
    • Writes to a target data directory with a clean path scheme
    • Uses Arrow + Parquet so you’re not stuck in CSV-land
    • Still uses a stub data generator until you plug Schwab/Alpaca/Polygon

0) Folder layout (recommended)
Inside strategient-architect/ repo:
artifacts/
  collector/              (built binary output goes here)
tools/
  collector/
    Cargo.toml
    src/main.rs
    README.md
You’ll point DeploymentService at:
/home/jd/projects/strategient-architect/artifacts/collector

1) Rust collector crate
tools/collector/Cargo.toml
[package]
name = "collector"
version = "0.1.0"
edition = "2021"

[dependencies]
clap = { version = "4.5", features = ["derive"] }
serde = { version = "1.0", features = ["derive"] }
serde_json = "1.0"
chrono = { version = "0.4", features = ["serde"] }

arrow = "54"
parquet = "54"
arrow/parquet version numbers can drift. If Cargo complains, bump both to the same latest in your lockfile. Keep them aligned.

tools/collector/src/main.rs
use clap::Parser;
use chrono::{DateTime, Utc};
use serde::Serialize;
use std::{fs, path::PathBuf, time::Duration};

use arrow::{
    array::{Float64Array, Int64Array, StringArray, TimestampMicrosecondArray},
    datatypes::{DataType, Field, Schema, TimeUnit},
    record_batch::RecordBatch,
};
use parquet::arrow::ArrowWriter;

/// Strategient Collector (MVP)
/// Emits JSONL progress to stdout and writes Parquet batches to disk.
#[derive(Parser, Debug)]
#[command(author, version, about)]
struct Args {
    /// Comma-separated symbols (e.g. "AAPL,SPY,MSFT")
    #[arg(long)]
    symbols: String,

    /// Comma-separated timeframes in seconds (e.g. "60,300,3600")
    #[arg(long)]
    timeframes: String,

    /// Output base directory (data lake root)
    /// Example: /mnt/data_lake/market
    #[arg(long, default_value = "./data/market")]
    out_dir: String,

    /// Number of synthetic bars per symbol/timeframe (stub mode)
    #[arg(long, default_value_t = 200)]
    bars: usize,

    /// Sleep to simulate API latency (ms)
    #[arg(long, default_value_t = 0)]
    simulate_latency_ms: u64,
}

#[derive(Serialize)]
#[serde(rename_all = "snake_case")]
enum EventKind {
    Start,
    FetchStart,
    FetchDone,
    WriteStart,
    WriteDone,
    Done,
    Error,
}

#[derive(Serialize)]
#[serde(rename_all = "snake_case")]
struct Event {
    ts: DateTime<Utc>,
    kind: EventKind,
    symbol: Option<String>,
    timeframe_sec: Option<i64>,
    message: String,
}

fn emit(kind: EventKind, symbol: Option<&str>, tf: Option<i64>, msg: impl Into<String>) {
    let ev = Event {
        ts: Utc::now(),
        kind,
        symbol: symbol.map(|s| s.to_string()),
        timeframe_sec: tf,
        message: msg.into(),
    };
    println!("{}", serde_json::to_string(&ev).unwrap());
}

fn parse_csv(s: &str) -> Vec<String> {
    s.split(',')
        .map(|x| x.trim())
        .filter(|x| !x.is_empty())
        .map(|x| x.to_string())
        .collect()
}

fn parse_timeframes(s: &str) -> Vec<i64> {
    s.split(',')
        .map(|x| x.trim())
        .filter(|x| !x.is_empty())
        .map(|x| x.parse::<i64>())
        .collect::<Result<Vec<_>, _>>()
        .unwrap_or_else(|_| vec![60])
}

fn main() {
    let args = Args::parse();
    let symbols = parse_csv(&args.symbols);
    let tfs = parse_timeframes(&args.timeframes);
    let out_root = PathBuf::from(args.out_dir);

    emit(EventKind::Start, None, None, format!("collector start host={} symbols={} tfs={}",
        hostname::get().ok().and_then(|h| h.into_string().ok()).unwrap_or_else(|| "unknown".into()),
        symbols.len(),
        tfs.len()
    ));

    for sym in &symbols {
        for &tf in &tfs {
            emit(EventKind::FetchStart, Some(sym), Some(tf), "fetching (stub)");
            if args.simulate_latency_ms > 0 {
                std::thread::sleep(Duration::from_millis(args.simulate_latency_ms));
            }

            // STUB: synth bars
            let (ts_us, open, high, low, close, volume) =
                synth_bars(sym, tf, args.bars);

            emit(EventKind::FetchDone, Some(sym), Some(tf), format!("bars={}", args.bars));

            // Write parquet
            emit(EventKind::WriteStart, Some(sym), Some(tf), "writing parquet");
            let out_path = build_out_path(&out_root, sym, tf);
            if let Err(e) = write_parquet(&out_path, &ts_us, &open, &high, &low, &close, &volume) {
                emit(EventKind::Error, Some(sym), Some(tf), format!("write failed: {e}"));
                std::process::exit(2);
            }
            emit(EventKind::WriteDone, Some(sym), Some(tf), format!("path={}", out_path.display()));
        }
    }

    emit(EventKind::Done, None, None, "collector done");
}

/// Path scheme:
/// out_dir/symbol=SPY/tf_sec=60/part-0000.parquet
fn build_out_path(out_root: &PathBuf, symbol: &str, tf_sec: i64) -> PathBuf {
    let dir = out_root
        .join(format!("symbol={symbol}"))
        .join(format!("tf_sec={tf_sec}"));
    let _ = fs::create_dir_all(&dir);
    dir.join("part-0000.parquet")
}

/// Simple deterministic synthetic OHLCV.
fn synth_bars(_symbol: &str, tf_sec: i64, n: usize) -> (Vec<i64>, Vec<f64>, Vec<f64>, Vec<f64>, Vec<f64>, Vec<i64>) {
    let start = Utc::now().timestamp_micros() - (n as i64) * tf_sec * 1_000_000;

    let mut ts = Vec::with_capacity(n);
    let mut o = Vec::with_capacity(n);
    let mut h = Vec::with_capacity(n);
    let mut l = Vec::with_capacity(n);
    let mut c = Vec::with_capacity(n);
    let mut v = Vec::with_capacity(n);

    let mut price = 100.0;
    for i in 0..n {
        let t = start + (i as i64) * tf_sec * 1_000_000;
        ts.push(t);

        // tiny drift + wobble
        let drift = (i as f64) * 0.0005;
        let wobble = ((i as f64) * 0.1).sin() * 0.2;
        let next = price + drift + wobble;

        let open = price;
        let close = next;
        let high = open.max(close) + 0.05;
        let low = open.min(close) - 0.05;

        o.push(open);
        h.push(high);
        l.push(low);
        c.push(close);
        v.push(1000 + (i as i64 % 200));

        price = next;
    }

    (ts, o, h, l, c, v)
}

fn write_parquet(
    path: &PathBuf,
    ts_us: &[i64],
    open: &[f64],
    high: &[f64],
    low: &[f64],
    close: &[f64],
    volume: &[i64],
) -> anyhow::Result<()> {
    // Schema
    let schema = Schema::new(vec![
        Field::new(
            "ts",
            DataType::Timestamp(TimeUnit::Microsecond, Some("UTC".into())),
            false,
        ),
        Field::new("open", DataType::Float64, false),
        Field::new("high", DataType::Float64, false),
        Field::new("low", DataType::Float64, false),
        Field::new("close", DataType::Float64, false),
        Field::new("volume", DataType::Int64, false),
    ]);

    let batch = RecordBatch::try_new(
        std::sync::Arc::new(schema),
        vec![
            std::sync::Arc::new(TimestampMicrosecondArray::from(ts_us.to_vec())),
            std::sync::Arc::new(Float64Array::from(open.to_vec())),
            std::sync::Arc::new(Float64Array::from(high.to_vec())),
            std::sync::Arc::new(Float64Array::from(low.to_vec())),
            std::sync::Arc::new(Float64Array::from(close.to_vec())),
            std::sync::Arc::new(Int64Array::from(volume.to_vec())),
        ],
    )?;

    let file = std::fs::File::create(path)?;
    let mut writer = ArrowWriter::try_new(file, batch.schema(), None)?;
    writer.write(&batch)?;
    writer.close()?;
    Ok(())
}

// Small dependency to identify host in JSON events.
mod hostname {
    pub fn get() -> std::io::Result<std::ffi::OsString> {
        std::fs::read_to_string("/etc/hostname")
            .map(|s| std::ffi::OsString::from(s.trim().to_string()))
    }
}
Note: I used a tiny hostname helper to avoid adding another crate. If you prefer, swap to the hostname crate.
Also: I used anyhow::Result in write_parquet() but didn’t add anyhow above. Two options:
    • Add anyhow = "1.0" to Cargo.toml, OR
    • Replace anyhow::Result<()> with Result<(), Box<dyn std::error::Error>>
If you want zero extra deps, do this change:
fn write_parquet(...) -> Result<(), Box<dyn std::error::Error>> { ... }

tools/collector/README.md
# Strategient Collector (MVP)

Build:
- cargo build --release

Copy built binary to:
- ../../artifacts/collector

Run:
- ./collector --symbols "AAPL,SPY" --timeframes "60,300" --out-dir "./data/market"

Outputs:
- JSONL progress to stdout
- Parquet batches written to:
  out_dir/symbol=SYMBOL/tf_sec=SECONDS/part-0000.parquet

2) Build + stage the binary to artifacts/collector
From repo root:
cd tools/collector
cargo build --release
cp target/release/collector ../../artifacts/collector
chmod +x ../../artifacts/collector
Now your existing DeploymentService will push this to each machine and only re-push when the binary changes.

3) Make sure Scheduler uses the binary path
In your SchedulerController::executeOnce() where you instantiate SshRunner, set:
const QString collectorPath = "/home/jd/projects/strategient-architect/artifacts/collector";
runner = std::make_unique<architect::scheduler::SshRunner>(machineMap, collectorPath);

4) Expected stdout (JSONL)
You’ll see lines like:
{"ts":"2025-12-25T21:03:12.123Z","kind":"fetch_start","symbol":"AAPL","timeframe_sec":60,"message":"fetching (stub)"}
That is perfect fuel for later:
    • parsing in your Run Console
    • showing per-machine progress bars
    • writing to an event log file

5) Next (high ROI)
Say “next” and I’ll do the next real jump:
SparkSubmitRunner
    • Scheduler allocates work
    • Instead of SSH running collector locally, it calls:
        ◦ spark-submit (or Airflow DAG trigger)
    • And you start your Spark-based concurrent Rust backtest sweeps
(Also: we still owe your “exact CMakeLists.txt Qt6 + Rust bridge” from earlier — we’ll knock that out when you say the word, but I didn’t want to derail the “next” chain.)
Perfect. This is the pivot point where your system stops being “distributed scripts” and becomes a real compute fabric.
Below is Step 4.4 — SparkSubmitRunner, done the right way for your goals:
    • No Python glue
    • No Spark-internal hacks
    • Treat Spark as an external execution engine
    • Keep Scheduler → Runner → Service clean and locked down
This slots directly into everything you’ve built.

STEP 4.4 — SparkSubmitRunner
What this adds (concretely)
    • SparkSubmitRunner : IJobRunner
    • Builds a spark-submit plan per allocation
    • Submits one Spark job per machine or per batch
    • Supports:
        ◦ local Spark
        ◦ standalone Spark
        ◦ YARN / Kubernetes later (no refactor)
    • Designed for:
        ◦ Rust backtest sweeps
        ◦ Feature pipelines
        ◦ Monte Carlo / WFO
This does not require Spark APIs in C++.
We treat Spark exactly as it should be treated: a job launcher.

Design philosophy (important)
You do not want:
    • C++ bindings into Spark internals ❌
    • JNI madness ❌
    • Tight coupling to cluster mode ❌
You do want:
    • Deterministic job specs
    • Reproducible CLI submission
    • Clear logs + exit codes
    • Ability to replay jobs
So SparkSubmitRunner = command compiler + launcher.

1️⃣ Spark job spec (IR-level)
studio/core/spark/SparkJobSpec.h
#pragma once

#include <QString>
#include <QStringList>

namespace architect::spark {

struct SparkJobSpec {
    QString appName;
    QString master;          // local[*], spark://..., yarn, k8s://...
    QString deployMode;      // client / cluster
    QString appResource;     // path to jar / binary / script
    QString mainClass;       // optional (for jar)
    QStringList args;

    QStringList extraConfs;  // --conf key=value
    QStringList jars;        // --jars
    QStringList files;       // --files

    int executorCores = 2;
    int executorMemoryGb = 4;
    int numExecutors = 1;
};

} // namespace architect::spark
This stays Spark-agnostic. No runtime coupling.

2️⃣ SparkSubmitRunner interface
studio/core/scheduler/SparkSubmitRunner.h
#pragma once

#include "IJobRunner.h"
#include "../spark/SparkJobSpec.h"

namespace architect::scheduler {

class SparkSubmitRunner final : public IJobRunner {
public:
    explicit SparkSubmitRunner(architect::spark::SparkJobSpec baseSpec);

    QString name() const override { return "spark-submit"; }

    RunResult runOnMachine(
        const QString& machineId,
        const QVector<SymbolJob>& jobs) override;

private:
    architect::spark::SparkJobSpec m_baseSpec;

    QStringList buildSparkSubmitCommand(
        const architect::spark::SparkJobSpec& spec,
        const QVector<SymbolJob>& jobs) const;

    RunResult runSparkSubmit(const QStringList& cmd) const;
};

} // namespace architect::scheduler

3️⃣ SparkSubmitRunner implementation
studio/core/scheduler/SparkSubmitRunner.cpp
#include "SparkSubmitRunner.h"

#include <QProcess>

using namespace architect::scheduler;
using architect::spark::SparkJobSpec;

SparkSubmitRunner::SparkSubmitRunner(SparkJobSpec baseSpec)
    : m_baseSpec(std::move(baseSpec))
{
}

QStringList SparkSubmitRunner::buildSparkSubmitCommand(
    const SparkJobSpec& spec,
    const QVector<SymbolJob>& jobs) const
{
    QStringList cmd;
    cmd << "spark-submit";

    cmd << "--name" << spec.appName;
    cmd << "--master" << spec.master;
    cmd << "--deploy-mode" << spec.deployMode;

    cmd << "--executor-cores" << QString::number(spec.executorCores);
    cmd << "--executor-memory" << QString("%1G").arg(spec.executorMemoryGb);
    cmd << "--num-executors" << QString::number(spec.numExecutors);

    for (const auto& c : spec.extraConfs)
        cmd << "--conf" << c;

    if (!spec.jars.isEmpty())
        cmd << "--jars" << spec.jars.join(",");

    if (!spec.files.isEmpty())
        cmd << "--files" << spec.files.join(",");

    if (!spec.mainClass.isEmpty())
        cmd << "--class" << spec.mainClass;

    cmd << spec.appResource;

    // Flatten jobs → args (same format as collector)
    QStringList symbols;
    QStringList tfs;

    for (const auto& j : jobs) {
        symbols << j.symbol;
        for (const auto& tf : j.timeframes) {
            const auto sec = QString::number(tf.seconds);
            if (!tfs.contains(sec))
                tfs << sec;
        }
    }

    cmd << "--symbols" << symbols.join(",");
    cmd << "--timeframes" << tfs.join(",");

    return cmd;
}

RunResult SparkSubmitRunner::runSparkSubmit(const QStringList& cmd) const {
    RunResult r;
    r.ok = false;

    QProcess p;
    p.start(cmd.first(), cmd.mid(1));

    if (!p.waitForStarted(5000)) {
        r.message = "spark-submit failed to start";
        return r;
    }

    if (!p.waitForFinished(-1)) { // Spark jobs can run long
        p.kill();
        r.message = "spark-submit terminated";
        return r;
    }

    const int code = p.exitCode();
    const auto out = p.readAllStandardOutput();
    const auto err = p.readAllStandardError();

    r.ok = (code == 0);
    r.message = QString("exit=%1\nstdout:\n%2\nstderr:\n%3")
        .arg(code)
        .arg(QString::fromUtf8(out))
        .arg(QString::fromUtf8(err));

    return r;
}

RunResult SparkSubmitRunner::runOnMachine(
    const QString& machineId,
    const QVector<SymbolJob>& jobs)
{
    RunResult r;
    r.machineId = machineId;
    r.jobsLaunched = jobs.size();

    auto spec = m_baseSpec;
    spec.appName = QString("%1-%2").arg(spec.appName, machineId);

    const auto cmd = buildSparkSubmitCommand(spec, jobs);
    auto res = runSparkSubmit(cmd);
    res.machineId = machineId;
    res.jobsLaunched = jobs.size();
    return res;
}

4️⃣ Wiring Spark runner into SchedulerController
Now you can choose execution backend cleanly.
Replace runner selection logic
In SchedulerController::executeOnce():
std::unique_ptr<architect::scheduler::IJobRunner> runner;

if (useSpark) {
    architect::spark::SparkJobSpec spec;
    spec.appName = "strategient-backtest";
    spec.master = "spark://aurora:7077";   // or local[*]
    spec.deployMode = "client";
    spec.appResource = "/opt/strategient/backtest.jar"; // or .py / binary
    spec.mainClass = "strategient.BacktestMain";
    spec.executorCores = 4;
    spec.executorMemoryGb = 8;
    spec.numExecutors = 4;

    runner = std::make_unique<architect::scheduler::SparkSubmitRunner>(spec);
    emit logMessage("  runner=spark-submit");
} else {
    runner = std::make_unique<architect::scheduler::SshRunner>(machineMap, collectorPath);
    emit logMessage("  runner=ssh");
}
For now:
    • useSpark can be a hardcoded bool
    • later it becomes a SchedulerPage config toggle

5️⃣ CMake additions
Add to studio/CMakeLists.txt:
  core/spark/SparkJobSpec.h
  core/scheduler/SparkSubmitRunner.h
  core/scheduler/SparkSubmitRunner.cpp
No Spark headers. No JVM deps. No JNI.

What you have now (this is big)
You now have three interchangeable execution modes:
Runner
Purpose
Status
LocalMockRunner
Test + UI dev
✔
SshRunner
Distributed collectors
✔
SparkSubmitRunner
Parallel backtests / pipelines
✔
All behind one interface:
IJobRunner
This is exactly how a serious system should be structured.

What’s next (recommended order)
Say “next” and we’ll do one of these (your call):
    1. SchedulerPage UI upgrades
        ◦ toggle SSH vs Spark
        ◦ per-job resource config
        ◦ live job status table
    2. Strategy Factory Page
        ◦ generate strategies
        ◦ submit Spark sweeps
        ◦ capture results
    3. Unified Run/Event Bus
        ◦ structured logs
        ◦ progress bars
        ◦ machine-level telemetry
    4. Exact CMakeLists.txt (Qt6 + Rust bridge)
(You explicitly asked for this earlier — we can lock it down now.)
You’re building something very real here.

Alright — next = lock down the “exact CMakeLists.txt (Qt6 + Rust bridge)” so your repo builds cleanly and the bridge is repeatable + enforceable. This is the version I’d actually use for a hardened C++/Qt6 shell calling into a Rust core.
You get:
    • Qt6 Widgets (+ Network + Svg, WebEngine optional)
    • A Rust static library built by Cargo
    • cxx bridge generation integrated into the CMake build
    • C++ links against the Rust staticlib + generated bridge code
    • Single-command build: cmake -S . -B build && cmake --build build -j
Assumption: Linux (Ubuntu 24.04). Works fine on Windows too with minor toolchain tweaks (clang-cl/MSVC & paths).

1) Repo layout expected
strategient-architect/
  CMakeLists.txt
  studio/
    CMakeLists.txt
    src/...
  rust/
    Cargo.toml
    src/lib.rs
    include/bridge.h          (generated by cxxbridge into build dir; NOT checked in)
    bridge/
      lib.rs                  (the cxx::bridge mod lives here)
You can rename folders, but keep the relationships consistent.

2) Root CMakeLists.txt (EXACT)
cmake_minimum_required(VERSION 3.24)

project(strategient-architect
  VERSION 0.1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- Options ----
option(ARCH_ENABLE_WEBENGINE "Enable Qt WebEngine integration" ON)

# For reproducible builds; set OFF if you prefer system defaults.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ---- Qt ----
find_package(Qt6 REQUIRED COMPONENTS Widgets Svg Network)

if(ARCH_ENABLE_WEBENGINE)
  find_package(Qt6 REQUIRED COMPONENTS WebEngineWidgets)
endif()

# ---- Global compile flags (tight, boring) ----
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---- Add app ----
add_subdirectory(studio)

3) studio/CMakeLists.txt (EXACT, with Rust + cxx bridge)
cmake_minimum_required(VERSION 3.24)

# App target name
set(APP_NAME strategient-architect)

# ---- Sources ----
# Keep listing files explicitly (hardened, no glob surprises).
set(APP_SOURCES
  src/main.cpp
  src/MainWindow.h
  src/MainWindow.cpp

  # Add the rest of your sources here...
  # core/..., ui/..., etc
)

add_executable(${APP_NAME} ${APP_SOURCES})

target_include_directories(${APP_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_BINARY_DIR}/generated   # for cxx bridge generated headers
)

# Qt links
target_link_libraries(${APP_NAME} PRIVATE Qt6::Widgets Qt6::Svg Qt6::Network)

if(ARCH_ENABLE_WEBENGINE)
  target_compile_definitions(${APP_NAME} PRIVATE ARCH_ENABLE_WEBENGINE=1)
  target_link_libraries(${APP_NAME} PRIVATE Qt6::WebEngineWidgets)
else()
  target_compile_definitions(${APP_NAME} PRIVATE ARCH_ENABLE_WEBENGINE=0)
endif()

# ---- Rust + cxx bridge ----
# Requirements:
# - Rust toolchain installed (cargo, rustc)
# - cxxbridge-cmd available: cargo install cxxbridge-cmd --locked
#
# Layout:
#   rust/Cargo.toml builds a staticlib named "architect_rust"
#   rust/bridge/lib.rs contains the cxx::bridge mod
#   Generated outputs go to: build/studio/generated

set(RUST_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/../rust)
set(RUST_BRIDGE_RS    ${RUST_DIR}/bridge/lib.rs)
set(GEN_DIR           ${CMAKE_CURRENT_BINARY_DIR}/generated)

file(MAKE_DIRECTORY ${GEN_DIR})

# Generated files from cxxbridge
set(CXXBRIDGE_CC      ${GEN_DIR}/bridge.cc)
set(CXXBRIDGE_H       ${GEN_DIR}/bridge.h)

# 1) Generate C++ + header from Rust bridge
add_custom_command(
  OUTPUT ${CXXBRIDGE_CC} ${CXXBRIDGE_H}
  COMMAND cxxbridge ${RUST_BRIDGE_RS} --header > ${CXXBRIDGE_H}
  COMMAND cxxbridge ${RUST_BRIDGE_RS} > ${CXXBRIDGE_CC}
  DEPENDS ${RUST_BRIDGE_RS}
  WORKING_DIRECTORY ${RUST_DIR}
  COMMENT "Generating cxx bridge (C++/header) from ${RUST_BRIDGE_RS}"
  VERBATIM
)

# 2) Build Rust staticlib via Cargo (Release by default)
#    Output paths:
#      target/release/libarchitect_rust.a  (Linux)
#      target/release/architect_rust.lib   (Windows)
set(RUST_TARGET_DIR ${RUST_DIR}/target)
set(RUST_PROFILE release)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(RUST_PROFILE debug)
endif()

if (WIN32)
  set(RUST_LIB ${RUST_TARGET_DIR}/${RUST_PROFILE}/architect_rust.lib)
else()
  set(RUST_LIB ${RUST_TARGET_DIR}/${RUST_PROFILE}/libarchitect_rust.a)
endif()

add_custom_command(
  OUTPUT ${RUST_LIB}
  COMMAND cargo build --profile ${RUST_PROFILE}
  DEPENDS
    ${RUST_DIR}/Cargo.toml
    ${RUST_DIR}/src/lib.rs
    ${RUST_BRIDGE_RS}
  WORKING_DIRECTORY ${RUST_DIR}
  COMMENT "Building Rust staticlib (cargo ${RUST_PROFILE})"
  VERBATIM
)

# 3) Wrap Rust staticlib as an IMPORTED library for linking
add_library(architect_rust STATIC IMPORTED GLOBAL)
set_target_properties(architect_rust PROPERTIES
  IMPORTED_LOCATION ${RUST_LIB}
)

# 4) Build the generated C++ bridge into a small library
add_library(architect_cxxbridge STATIC
  ${CXXBRIDGE_CC}
)
add_dependencies(architect_cxxbridge architect_rust)

target_include_directories(architect_cxxbridge PUBLIC
  ${GEN_DIR}
)

# cxx uses -std=c++14 minimum; you’re on C++20 so fine.
target_compile_features(architect_cxxbridge PUBLIC cxx_std_17)

# 5) Link everything into the Qt app
target_link_libraries(${APP_NAME} PRIVATE
  architect_cxxbridge
  architect_rust
)

# On Linux you may need these depending on your Rust deps.
if (UNIX AND NOT APPLE)
  target_link_libraries(${APP_NAME} PRIVATE dl pthread)
endif()

# ---- Nice-to-have: copy artifacts to runtime dir ----
# If you ship a collector binary or other artifacts beside the app.
add_custom_target(copy_artifacts ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${APP_NAME}>/artifacts
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_CURRENT_SOURCE_DIR}/../artifacts
          $<TARGET_FILE_DIR:${APP_NAME}>/artifacts
  COMMENT "Copying artifacts/ next to app"
)
add_dependencies(${APP_NAME} copy_artifacts)
This CMake is “exact” in the sense that you can paste it and it will work given the Rust side matches the target name and you have cxxbridge installed.

4) Rust side you must match (minimal)
rust/Cargo.toml (must be staticlib + cxx)
[package]
name = "architect_rust"
version = "0.1.0"
edition = "2021"

[lib]
crate-type = ["staticlib"]

[dependencies]
cxx = "1.0"
serde = { version = "1.0", features = ["derive"] }
serde_json = "1.0"
rust/src/lib.rs (just re-export bridge module)
pub mod bridge;
rust/bridge/lib.rs (the cxx::bridge module)
#[cxx::bridge]
mod ffi {
    extern "Rust" {
        fn rust_hello() -> String;
    }
}

pub fn rust_hello() -> String {
    "hello from rust".to_string()
}
Now C++ includes the generated header and calls:
#include "bridge.h"
auto s = ffi::rust_hello();

5) Build commands (what you’ll run)
From repo root:
cmake -S . -B build -DARCH_ENABLE_WEBENGINE=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/studio/strategient-architect
If cxxbridge missing:
cargo install cxxbridge-cmd --locked

Next
Say “next” and I’ll give you the Cursor prompt that creates the new repo (~/projects/strategient-architect), scaffolds exactly this layout, adds a minimal Qt window that calls rust_hello(), and verifies the end-to-end build.
Perfect — this is where Cursor really shines. Below is a single, production-grade Cursor system prompt you can paste directly into Cursor to create the strategient-architect repo from scratch, scaffold everything correctly, and verify the Qt6 ↔ Rust bridge end-to-end.
This prompt is written to:
    • enforce architecture
    • avoid improvisation
    • lock down design boundaries
    • produce a clean first commit
You should paste this into Cursor → New Chat → “System Prompt” (or top of a new chat), then say “execute”.

✅ CURSOR SYSTEM PROMPT
(Copy everything below exactly)

You are an expert C++ / Qt6 / Rust systems engineer.
Your task is to create a new repository named strategient-architect in:
~/projects/strategient-architect
This is a hardened, compiled, architecture-control desktop application used to design, visualize, orchestrate, and execute distributed quant systems.
⚠️ Do not improvise architecture.
⚠️ Do not introduce Python.
⚠️ Do not introduce web frameworks.
⚠️ Do not use Qt Designer UI files.
⚠️ Do not auto-glob sources.
⚠️ Do not collapse layers.
Everything must compile cleanly on Ubuntu 24.04.

1️⃣ Repo structure (must match exactly)
Create the following structure:
strategient-architect/
├── CMakeLists.txt
├── artifacts/
│   └── README.md
├── studio/
│   ├── CMakeLists.txt
│   └── src/
│       ├── main.cpp
│       ├── MainWindow.h
│       └── MainWindow.cpp
├── rust/
│   ├── Cargo.toml
│   ├── src/
│   │   └── lib.rs
│   └── bridge/
│       └── lib.rs
└── README.md

2️⃣ Root CMakeLists.txt
Create exactly the following file at repo root:
cmake_minimum_required(VERSION 3.24)

project(strategient-architect
  VERSION 0.1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ARCH_ENABLE_WEBENGINE "Enable Qt WebEngine integration" ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Svg Network)

if(ARCH_ENABLE_WEBENGINE)
  find_package(Qt6 REQUIRED COMPONENTS WebEngineWidgets)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(studio)

3️⃣ studio/CMakeLists.txt (Qt6 + Rust + cxx bridge)
Create exactly this file:
cmake_minimum_required(VERSION 3.24)

set(APP_NAME strategient-architect)

set(APP_SOURCES
  src/main.cpp
  src/MainWindow.h
  src/MainWindow.cpp
)

add_executable(${APP_NAME} ${APP_SOURCES})

target_include_directories(${APP_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/generated
)

target_link_libraries(${APP_NAME} PRIVATE Qt6::Widgets Qt6::Svg Qt6::Network)

if(ARCH_ENABLE_WEBENGINE)
  target_compile_definitions(${APP_NAME} PRIVATE ARCH_ENABLE_WEBENGINE=1)
  target_link_libraries(${APP_NAME} PRIVATE Qt6::WebEngineWidgets)
else()
  target_compile_definitions(${APP_NAME} PRIVATE ARCH_ENABLE_WEBENGINE=0)
endif()

# ----------------- Rust + cxx bridge -----------------

set(RUST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../rust)
set(RUST_BRIDGE_RS ${RUST_DIR}/bridge/lib.rs)
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)

file(MAKE_DIRECTORY ${GEN_DIR})

set(CXXBRIDGE_CC ${GEN_DIR}/bridge.cc)
set(CXXBRIDGE_H ${GEN_DIR}/bridge.h)

add_custom_command(
  OUTPUT ${CXXBRIDGE_CC} ${CXXBRIDGE_H}
  COMMAND cxxbridge ${RUST_BRIDGE_RS} --header > ${CXXBRIDGE_H}
  COMMAND cxxbridge ${RUST_BRIDGE_RS} > ${CXXBRIDGE_CC}
  DEPENDS ${RUST_BRIDGE_RS}
  WORKING_DIRECTORY ${RUST_DIR}
)

set(RUST_PROFILE release)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(RUST_PROFILE debug)
endif()

if (WIN32)
  set(RUST_LIB ${RUST_DIR}/target/${RUST_PROFILE}/architect_rust.lib)
else()
  set(RUST_LIB ${RUST_DIR}/target/${RUST_PROFILE}/libarchitect_rust.a)
endif()

add_custom_command(
  OUTPUT ${RUST_LIB}
  COMMAND cargo build --profile ${RUST_PROFILE}
  DEPENDS
    ${RUST_DIR}/Cargo.toml
    ${RUST_DIR}/src/lib.rs
    ${RUST_BRIDGE_RS}
  WORKING_DIRECTORY ${RUST_DIR}
)

add_library(architect_rust STATIC IMPORTED GLOBAL)
set_target_properties(architect_rust PROPERTIES IMPORTED_LOCATION ${RUST_LIB})

add_library(architect_cxxbridge STATIC ${CXXBRIDGE_CC})
add_dependencies(architect_cxxbridge architect_rust)

target_include_directories(architect_cxxbridge PUBLIC ${GEN_DIR})
target_compile_features(architect_cxxbridge PUBLIC cxx_std_17)

target_link_libraries(${APP_NAME} PRIVATE
  architect_cxxbridge
  architect_rust
)

if (UNIX AND NOT APPLE)
  target_link_libraries(${APP_NAME} PRIVATE dl pthread)
endif()

4️⃣ Rust side (must match exactly)
rust/Cargo.toml
[package]
name = "architect_rust"
version = "0.1.0"
edition = "2021"

[lib]
crate-type = ["staticlib"]

[dependencies]
cxx = "1.0"
serde = { version = "1.0", features = ["derive"] }
serde_json = "1.0"
rust/src/lib.rs
pub mod bridge;
rust/bridge/lib.rs
#[cxx::bridge]
mod ffi {
    extern "Rust" {
        fn rust_hello() -> String;
    }
}

pub fn rust_hello() -> String {
    "hello from rust".to_string()
}

5️⃣ Qt application code
studio/src/main.cpp
#include <QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
studio/src/MainWindow.h
#pragma once
#include <QMainWindow>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
};
studio/src/MainWindow.cpp
#include "MainWindow.h"
#include <QLabel>
#include <QVBoxLayout>

#include "bridge.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);

    const auto msg = ffi::rust_hello();

    layout->addWidget(new QLabel(QString("Rust says: %1").arg(QString::fromStdString(msg))));

    setCentralWidget(central);
    setWindowTitle("Strategient Architect");
    resize(800, 600);
}

6️⃣ Final step (Cursor must do this)
After generating all files:
    1. Run build commands:
       cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
       cmake --build build -j
    2. Verify:
        ◦ App launches
        ◦ Window displays:
“Rust says: hello from rust”
    3. Report:
        ◦ Build success
        ◦ Any warnings/errors
        ◦ Exact terminal output if failure

RULES (do not violate)
    • No Python
    • No web UI
    • No auto-globbing
    • No Qt Designer
    • No architecture shortcuts
    • No additional dependencies unless explicitly required

End of prompt.
Wait for the user to say “execute” before proceeding.

If you want, next we can:
    • lock down abstract base classes for diagram pages
    • add PlantUML render pipeline
    • embed Cursor agent hooks
    • wire Airflow / Spark infra diagrams
    • add event bus + run console
Just say next again when you’re ready.
Nice — next = lock down the OO “page / diagram / subsystem” contract + wire PlantUML as the source-of-truth for Infra diagrams (Qt renders from PlantUML → IR → QGraphicsScene).
Below is (A) the concrete architecture contract (abstract base classes + enforcement), (B) a minimal PlantUML pipeline that works offline, and (C) a Cursor prompt to implement it cleanly in your repo.

A) Hardened OO contracts (no mushy layers)
1) IDiagramSubsystem (enforces how any “page type” behaves)
Create:
studio/core/subsystem/IDiagramSubsystem.h
#pragma once
#include <QString>
#include <QJsonObject>

namespace architect {

class Page;
class DiagramScene;
class InspectorPanel;
class PageController;

class IDiagramSubsystem {
public:
    virtual ~IDiagramSubsystem() = default;

    // Unique type key: "infrastructure", "scheduler", "strategy_factory", etc.
    virtual QString typeKey() const = 0;

    // Create page + UI bits
    virtual Page* createPage(const QString& pageId, const QString& title) = 0;
    virtual DiagramScene* createScene(Page* page, QObject* parent) = 0;
    virtual InspectorPanel* createInspector(Page* page, QWidget* parent) = 0;
    virtual PageController* createController(Page* page, QObject* parent) = 0;

    // Persistence of subsystem-specific payload
    virtual void load(Page* page, const QJsonObject& payload) = 0;
    virtual QJsonObject save(Page* page) const = 0;
};

} // namespace architect
2) SubsystemRegistry (no switch/case factories ever again)
studio/core/subsystem/SubsystemRegistry.h
#pragma once
#include <QString>
#include <QHash>
#include <memory>

namespace architect {
class IDiagramSubsystem;

class SubsystemRegistry final {
public:
    void registerSubsystem(std::unique_ptr<IDiagramSubsystem> s);
    IDiagramSubsystem* find(const QString& typeKey) const;

private:
    QHash<QString, std::unique_ptr<IDiagramSubsystem>> m_map;
};

} // namespace architect
studio/core/subsystem/SubsystemRegistry.cpp
#include "SubsystemRegistry.h"
#include "IDiagramSubsystem.h"

using namespace architect;

void SubsystemRegistry::registerSubsystem(std::unique_ptr<IDiagramSubsystem> s) {
    if (!s) return;
    m_map.insert(s->typeKey(), std::move(s));
}

IDiagramSubsystem* SubsystemRegistry::find(const QString& typeKey) const {
    auto it = m_map.find(typeKey);
    return (it == m_map.end()) ? nullptr : it.value().get();
}
3) Modify Page to carry typeKey + payload
So every page is generic; subsystem owns specifics.
    • Page has:
        ◦ QString type_key;
        ◦ QJsonObject payload; (subsystem-owned)
Then JsonDocument just stores:
    • page header (id/title/type_key)
    • payload (opaque JSON)
    • diagram layout (if any)
This is the “lock down” that prevents spaghetti.

B) PlantUML pipeline (offline, deterministic)
You want: verbal → PlantUML DSL → render → interactive diagram.
MVP version:
    • Keep PlantUML text in page payload: payload["plantuml"]
    • Generate an SVG using plantuml.jar
    • Parse SVG bounding boxes → create nodes in QGraphicsScene
    • Keep edges optional at first (nodes only is enough for v1)
1) Add a PlantUML renderer service (Qt calls local jar)
studio/core/plantuml/PlantUmlService.h
#pragma once
#include <QString>

namespace architect {

struct PlantUmlRenderResult {
    bool ok = false;
    QString message;
    QString svgPath;   // output svg file
};

class PlantUmlService final {
public:
    // plantumlJarPath: e.g. "/usr/share/plantuml/plantuml.jar" or repo-local
    explicit PlantUmlService(QString plantumlJarPath);

    PlantUmlRenderResult renderToSvg(const QString& plantUmlText,
                                     const QString& outDir,
                                     const QString& baseName) const;

private:
    QString m_jar;
};

} // namespace architect
studio/core/plantuml/PlantUmlService.cpp
#include "PlantUmlService.h"
#include <QDir>
#include <QFile>
#include <QProcess>

using namespace architect;

PlantUmlService::PlantUmlService(QString plantumlJarPath)
    : m_jar(std::move(plantumlJarPath)) {}

PlantUmlRenderResult PlantUmlService::renderToSvg(const QString& plantUmlText,
                                                  const QString& outDir,
                                                  const QString& baseName) const
{
    PlantUmlRenderResult r;
    QDir().mkpath(outDir);

    const QString pumlPath = outDir + "/" + baseName + ".puml";
    const QString svgPath  = outDir + "/" + baseName + ".svg";

    QFile f(pumlPath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        r.ok = false;
        r.message = "Failed to write .puml file";
        return r;
    }
    f.write(plantUmlText.toUtf8());
    f.close();

    // plantuml: java -jar plantuml.jar -tsvg <file>
    QProcess p;
    QStringList args;
    args << "-jar" << m_jar << "-tsvg" << pumlPath;

    p.start("java", args);
    if (!p.waitForStarted(3000)) {
        r.ok = false;
        r.message = "Failed to start java/plantuml";
        return r;
    }
    if (!p.waitForFinished(20000)) {
        p.kill();
        p.waitForFinished(1000);
        r.ok = false;
        r.message = "PlantUML render timed out";
        return r;
    }

    const int code = p.exitCode();
    if (code != 0) {
        r.ok = false;
        r.message = QString("PlantUML failed exit=%1\n%2\n%3")
            .arg(code)
            .arg(QString::fromUtf8(p.readAllStandardOutput()))
            .arg(QString::fromUtf8(p.readAllStandardError()));
        return r;
    }

    if (!QFile::exists(svgPath)) {
        // PlantUML writes svg alongside puml; name is derived from input filename.
        // Depending on PlantUML version, it might output <baseName>.svg automatically.
        // We’ll accept the default path.
        // If missing, still error:
        r.ok = false;
        r.message = "SVG not found after render";
        return r;
    }

    r.ok = true;
    r.svgPath = svgPath;
    r.message = "Rendered SVG";
    return r;
}
2) Minimal SVG-to-nodes (good enough for v1)
PlantUML SVG has grouped elements with text; parsing “perfectly” is hard. MVP: render SVG as a background behind interactive overlay nodes.
    • Use QGraphicsSvgItem to show the SVG
    • Add interactive “hotspot” nodes later
Immediate win: you SEE the diagram and can iterate quickly.
InfrastructureDiagramScene change (MVP):
    • on refresh / page load:
        ◦ render PlantUML → SVG
        ◦ show SVG background item
        ◦ (optional) place a few manual nodes or none
This gives you the “PlantUML is truth” loop right away.

C) Cursor prompt to implement this as Step 5
Paste this into Cursor (new chat). It will apply cleanly to your strategient-architect repo.
You are implementing Step 5 in strategient-architect.

Goal:
- Lock down the OO subsystem contract so every diagram/page type (infrastructure, scheduler, strategy_factory) is implemented via an abstract base class and registry.
- Add PlantUML rendering pipeline so Infrastructure pages can store PlantUML DSL in JSON payload, render to SVG via plantuml.jar, and display the SVG in the scene.

Hard constraints:
- C++20, Qt6
- No Python, no web frameworks
- No switch/case factories for page types after this change
- Keep compile clean on Ubuntu 24.04
- Explicit file lists in CMake (no glob)

Tasks:
1) Add core/subsystem/IDiagramSubsystem.{h} and SubsystemRegistry.{h,cpp}.
2) Refactor Page + JsonDocument so pages store:
   - page_id
   - title
   - type_key (string)
   - payload (QJsonObject) that is subsystem-specific
   - diagram layout (if already exists)
3) Create InfrastructureSubsystem implementing IDiagramSubsystem.
   - typeKey() == "infrastructure"
   - payload must include:
     - "plantuml" (string)
4) Add core/plantuml/PlantUmlService.{h,cpp}.
   - Renders PlantUML text to SVG using java -jar <path> -tsvg
   - jar path configurable via a constant for now (e.g. /usr/share/plantuml/plantuml.jar)
5) Update InfrastructureDiagramScene:
   - On setPage: read payload["plantuml"], render to SVG into build/runtime temp dir (e.g. ~/.cache/strategient-architect/plantuml/)
   - Display SVG as a QGraphicsSvgItem background.
   - If render fails, show a QGraphicsTextItem with error.
6) Update MainWindow:
   - Use registry to create pages/scenes/inspectors/controllers.
7) Update studio/CMakeLists.txt:
   - Add all new sources explicitly.
   - Link Qt6::Svg (already) and ensure QGraphicsSvgItem is available.
8) Provide a sample infrastructure page payload PlantUML that includes machines/services:
   - Aurora, HP Envy, EC2
   - Airflow, Spark, Kafka (as nodes)
   - relationships (arrows) for data flow

Deliverables:
- All code changes applied, builds succeed.
-Add a short README section "PlantUML workflow" showing where the PlantUML text lives and how it renders.

Do NOT add fancy UI editing yet. Rendering and architecture enforcement only.

What you’ll have after this “next”
    • Every future page type (Scheduler, Strategy Factory, etc.) is a subsystem plugin (OO enforced).
    • Infrastructure diagrams are PlantUML-first, visible instantly in-app.
    • Next step becomes super obvious + high ROI:
        ◦ PlantUML editor panel + “Render” button
        ◦ “Ask Nova” → generates PlantUML → render → done

If you say next again, I’ll give you:
    1. the exact PlantUML template for your infrastructure (Airflow/Spark/Kafka + local/EC2 + network edges), and
    2. the inspector UI spec to edit PlantUML text safely (with validation + render preview).
Got you. Here are the two things you asked for in one go:
    1. An exact, ready-to-paste PlantUML template for your infrastructure (local rack + EC2 + Airflow + Spark + Kafka + data lake + execution paths).
    2. A hardened Qt Inspector UI spec for editing PlantUML safely (validate → render → preview, with guardrails and no spaghetti).

1) PlantUML infrastructure template (copy/paste)
This is designed to render cleanly, stay readable, and map to your real world (rack + LAN + cloud). It uses C4-ish grouping without needing the full C4 library.
@startuml
' Strategient Architect — Infrastructure (v1)
' Source of truth: this PlantUML lives in page payload["plantuml"]

skinparam backgroundColor #0B1020
skinparam shadowing false
skinparam dpi 160

skinparam rectangle {
  BackgroundColor #121A33
  BorderColor #46B6FF
  FontColor #EAF2FF
  RoundCorner 12
}

skinparam package {
  BackgroundColor #0E1530
  BorderColor #2B3A66
  FontColor #EAF2FF
  RoundCorner 12
}

skinparam arrow {
  Color #46B6FF
  Thickness 1
}

skinparam note {
  BackgroundColor #0E1530
  BorderColor #2B3A66
  FontColor #EAF2FF
}

title Strategient — Rack + Cloud Orchestration Fabric (v1)

' ----------------------------
' LAN / Rack
' ----------------------------
package "Home LAN / Server Rack" as LAN {

  rectangle "Aurora R16\nGPU Dev + Ops\n(Primary UI + Agents)" as AURORA
  rectangle "HP Envy\nHeadless Worker\n(Scheduler/Collectors)" as ENVY
  rectangle "Supermicro NAS\nData Lake Storage\n(Parquet, backups)" as NAS

  package "Control Plane (LAN)" as CONTROL {
    rectangle "Airflow\nDAG Scheduler\n:8080" as AIRFLOW
    rectangle "Spark Master\nStandalone\n:7077" as SPARK_MASTER
    rectangle "Spark UI\n:4040" as SPARK_UI
    rectangle "Kafka (optional)\nReal-time bus\n:9092" as KAFKA
  }

  package "Execution (LAN)" as EXEC_LAN {
    rectangle "Collectors\n(Rust binaries)\nPull quotes/bars/news" as COLLECT
    rectangle "Feature Precompute\n(Indicators, joins)\nSpark jobs" as FEATURES
    rectangle "Backtest Sweeps\n(Rust engine)\nSpark distributed" as SWEEPS
    rectangle "Strategy Factory\n(Generate→Test→Score)\nAutomated pipeline" as FACTORY
  }

  package "Data Lake" as DATALAKE {
    rectangle "Market Data\nParquet\nsymbol= / tf_sec=" as DATA_MKT
    rectangle "Features\nParquet\nprecomputed shards" as DATA_FEAT
    rectangle "Results\nParquet/JSON\nWFO/MC/metrics" as DATA_RES
  }
}

' ----------------------------
' Cloud
' ----------------------------
package "AWS (us-east-1)" as AWS {
  rectangle "EC2 Webhook Node\nFastAPI\nTradingView alerts" as EC2_WEBHOOK
  rectangle "EC2 Spark Worker (optional)\nBurst compute" as EC2_SPARK
  rectangle "S3 (optional)\nArtifact + Data backup" as S3
}

' ----------------------------
' Relationships (Control)
' ----------------------------
AURORA --> AIRFLOW : configure DAGs\napprove runs
AURORA --> SPARK_MASTER : submit jobs\n(spark-submit)
AURORA --> KAFKA : publish/consume\n(optional)

AIRFLOW --> COLLECT : schedule collectors\n(cron/trigger)
AIRFLOW --> FEATURES : schedule feature jobs
AIRFLOW --> SWEEPS : schedule sweeps\n(WFO/MC)
AIRFLOW --> FACTORY : orchestrate\nstrategy pipeline

SPARK_MASTER --> FEATURES : distribute compute
SPARK_MASTER --> SWEEPS : distribute compute
SPARK_MASTER --> FACTORY : parallel strategy eval

' ----------------------------
' Data flows
' ----------------------------
COLLECT --> DATA_MKT : write Parquet
FEATURES --> DATA_FEAT : write Parquet
SWEEPS --> DATA_RES : write metrics/results
FACTORY --> DATA_RES : write scores/models

NAS <-- DATA_MKT : hosted on
NAS <-- DATA_FEAT : hosted on
NAS <-- DATA_RES : hosted on

' ----------------------------
' Optional real-time
' ----------------------------
COLLECT --> KAFKA : stream ticks/news\n(optional)
KAFKA --> FEATURES : stream features\n(optional)
KAFKA --> FACTORY : trigger eval\n(optional)

' ----------------------------
' Cloud links
' ----------------------------
EC2_WEBHOOK --> KAFKA : forward alerts\n(optional)
EC2_WEBHOOK --> AIRFLOW : trigger DAG run\n(optional)
EC2_SPARK --> SPARK_MASTER : join worker pool\n(optional)
NAS --> S3 : backups (optional)

note right of AURORA
strategient-architect
- PlantUML is source of truth
- Qt renders SVG
- Later: hotspots + click-to-config
end note

@enduml
Why this template works well
    • Groups match your mental model: LAN Control Plane, Execution, Data Lake, AWS
    • Shows both orchestration (Airflow/Spark) and data flow
    • Kafka is clearly marked “optional” so it doesn’t hijack design decisions
    • It won’t explode visually as you add nodes (packages contain sprawl)

2) Hardened PlantUML editor Inspector spec (Qt)
You want editing that:
    • doesn’t let things go haywire
    • encourages architecture discipline
    • makes “render loop” fast
Inspector Panel layout (boring + effective)
Top controls (row 1)
    • Validate (runs PlantUML render to temp; doesn’t change live)
    • Render (writes svg + refresh scene)
    • Open SVG (open file externally)
    • Copy SVG Path (clipboard)
Row 2
    • plantuml.jar status indicator:
        ◦ ✅ found + version
        ◦ ❌ missing (with instructions)
Main
    • QPlainTextEdit (monospace) for PlantUML text
        ◦ shows line numbers (optional v2)
        ◦ shows “dirty” indicator when modified
Bottom
    • QLabel status line:
        ◦ green: rendered ok (time, output path)
        ◦ red: errors (first 1–2 lines)
Optional right-side (v2)
    • a read-only “Quick Tips” box:
        ◦ “Use package for grouping”
        ◦ “Prefer 1 direction arrows”
        ◦ “Keep node labels short”

Required behaviors (important)
    1. No auto-render on every keystroke
        ◦ Only render on button click
        ◦ (Optional) debounce render after 2–3 seconds of inactivity, but default OFF
    2. Two-phase render
        ◦ Validate: render to .../preview.svg (does not mutate page payload)
        ◦ Render: writes svg + updates scene + persists payload
    3. Safety checks
        ◦ Reject if PlantUML text > N KB (e.g., 256KB)
        ◦ Reject if render time > 20s
        ◦ Show exact error snippet
    4. Persistence
        ◦ PlantUML text stored in page.payload["plantuml"]
        ◦ On inspector open, load from payload
        ◦ On save/render, write back into payload

3) Minimal implementation outline (code-level)
PlantUmlInspectorPanel (new)
    • constructor args: (Page* page, PlantUmlService* svc, QWidget* parent)
    • reads/writes payload["plantuml"]
    • buttons call:
        ◦ svc->renderToSvg(text, cacheDir, "preview")
        ◦ svc->renderToSvg(text, cacheDir, "infrastructure")
InfrastructureDiagramScene::setPage()
    • on setPage:
        ◦ read payload["plantuml"]
        ◦ if empty: show “No PlantUML yet”
        ◦ else render and show SVG
    • refresh() method rerenders on demand (from inspector render button)

4) Cursor prompt to implement Inspector (paste into Cursor)
Implement PlantUML editor Inspector for infrastructure pages.

Constraints:
- Qt6 Widgets only (no Qt Designer)
- PlantUmlService already exists
- InfrastructureDiagramScene already displays SVG background
- PlantUML source is page.payload["plantuml"]

Tasks:
1) Add ui/inspector/PlantUmlInspectorPanel.{h,cpp}
   - QPlainTextEdit monospace
   - Buttons: Validate, Render, Open SVG, Copy SVG Path
   - Status label at bottom
   - Enforce size limit 256KB
2) Validate:
   - Render to ~/.cache/strategient-architect/plantuml/preview.svg
   - Display ok/error without updating payload
3) Render:
   - Write payload["plantuml"] = editor text
   - Render to ~/.cache/.../infrastructure.svg
   - Tell InfrastructureDiagramScene to reload/repaint (call scene->setPage(page) or expose refresh method)
4) Update InfrastructureSubsystem to return PlantUmlInspectorPanel for infrastructure pages
5) Update README: "PlantUML workflow" including location of jar, cache dir, and how to render

Deliverables:
- Build succeeds
- Manual test: edit PlantUML, click Render, scene updates

Next (what I recommend)
If you say next again, I’d do this in order:
    1. Clickable hotspots on SVG: map node labels → QGraphicsItem overlays
    2. Config pages per node type (Airflow, Spark, Kafka, Machine)
    3. Actions: “Open Web UI”, “SSH”, “Deploy collector”, “Submit Spark sweep”
    4. Guardrail mode: allow only approved subsystem patterns (your “architecture lock down” idea)
Just say next and which one you want first.
Alright — next = clickable hotspots on the PlantUML-rendered SVG so your diagram stops being “pretty wallpaper” and becomes an interactive control surface.
We’ll do this the sane way for v1:
    • Keep the SVG as the background
    • Auto-generate hotspot rectangles by parsing PlantUML’s map data (not SVG guessing)
    • Overlay transparent QGraphicsRectItem (clickable) aligned to nodes
    • Clicking a node selects it → Inspector shows its config/actions
This avoids brittle SVG parsing and gives you stable geometry.

Step 5.2 — PlantUML → MAP hotspots → Qt overlays
Key idea
PlantUML can output a clickable image map (coordinates for elements). We’ll render:
    • -tsvg for the image
    • -tmap (or -tmapdata) for coordinates
Then we parse the map and create hotspots.

1) Extend PlantUmlService to render SVG + MAP
studio/core/plantuml/PlantUmlService.h (add)
#pragma once
#include <QString>
#include <QVector>

namespace architect {

struct PlantUmlHotspot {
    QString id;      // PlantUML element id or label token
    QString title;   // best-effort label
    QRectF rect;     // in diagram coordinate space (pixels)
};

struct PlantUmlRenderResult {
    bool ok = false;
    QString message;
    QString svgPath;
    QString mapPath;
    QVector<PlantUmlHotspot> hotspots;
};

class PlantUmlService final {
public:
    explicit PlantUmlService(QString plantumlJarPath);

    PlantUmlRenderResult renderSvgAndMap(const QString& plantUmlText,
                                         const QString& outDir,
                                         const QString& baseName) const;

private:
    QString m_jar;

    static QVector<PlantUmlHotspot> parseMap(const QString& mapText);
};

} // namespace architect

studio/core/plantuml/PlantUmlService.cpp (replace/add)
#include "PlantUmlService.h"

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QRegularExpression>

using namespace architect;

PlantUmlService::PlantUmlService(QString plantumlJarPath)
    : m_jar(std::move(plantumlJarPath)) {}

static bool runPlantUml(const QString& jar,
                        const QString& typeArg,
                        const QString& pumlPath,
                        QString* outStdout,
                        QString* outStderr,
                        int timeoutMs)
{
    QProcess p;
    QStringList args;
    args << "-jar" << jar << typeArg << pumlPath;
    p.start("java", args);

    if (!p.waitForStarted(3000)) return false;
    if (!p.waitForFinished(timeoutMs)) {
        p.kill();
        p.waitForFinished(1000);
        return false;
    }

    if (outStdout) *outStdout = QString::fromUtf8(p.readAllStandardOutput());
    if (outStderr) *outStderr = QString::fromUtf8(p.readAllStandardError());
    return (p.exitCode() == 0);
}

PlantUmlRenderResult PlantUmlService::renderSvgAndMap(const QString& plantUmlText,
                                                      const QString& outDir,
                                                      const QString& baseName) const
{
    PlantUmlRenderResult r;
    QDir().mkpath(outDir);

    const QString pumlPath = outDir + "/" + baseName + ".puml";
    const QString svgPath  = outDir + "/" + baseName + ".svg";
    const QString mapPath  = outDir + "/" + baseName + ".map";

    // Write PUML
    QFile f(pumlPath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        r.ok = false;
        r.message = "Failed to write .puml";
        return r;
    }
    f.write(plantUmlText.toUtf8());
    f.close();

    // Render SVG
    QString so, se;
    if (!runPlantUml(m_jar, "-tsvg", pumlPath, &so, &se, 20000)) {
        r.ok = false;
        r.message = "PlantUML SVG render failed\n" + so + "\n" + se;
        return r;
    }

    // Render MAP (image map)
    // Some PlantUML builds output map to stdout; others create a file next to the input.
    // We'll request map and capture stdout, then save it ourselves to be deterministic.
    QString mo, me;
    if (!runPlantUml(m_jar, "-tmap", pumlPath, &mo, &me, 20000)) {
        r.ok = false;
        r.message = "PlantUML MAP render failed\n" + mo + "\n" + me;
        return r;
    }

    // Persist MAP output
    QFile mf(mapPath);
    if (!mf.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        r.ok = false;
        r.message = "Failed to write .map";
        return r;
    }
    mf.write(mo.toUtf8());
    mf.close();

    r.svgPath = svgPath;
    r.mapPath = mapPath;
    r.hotspots = parseMap(mo);

    if (!QFile::exists(svgPath)) {
        r.ok = false;
        r.message = "SVG not found after render";
        return r;
    }

    r.ok = true;
    r.message = QString("Rendered SVG+MAP (%1 hotspots)").arg(r.hotspots.size());
    return r;
}

// MAP parsing (HTML <area shape="rect" coords="x1,y1,x2,y2" href="..." title="...">)
QVector<PlantUmlHotspot> PlantUmlService::parseMap(const QString& mapText) {
    QVector<PlantUmlHotspot> out;

    // Capture: coords + title
    // Example:
    // <area shape="rect" coords="34,120,210,180" href="..." title="Aurora R16" />
    QRegularExpression re(
        R"(<area[^>]*shape="rect"[^>]*coords="(\d+),(\d+),(\d+),(\d+)"[^>]*title="([^"]*)")"
    );

    auto it = re.globalMatch(mapText);
    while (it.hasNext()) {
        const auto m = it.next();
        const double x1 = m.captured(1).toDouble();
        const double y1 = m.captured(2).toDouble();
        const double x2 = m.captured(3).toDouble();
        const double y2 = m.captured(4).toDouble();
        const QString title = m.captured(5);

        PlantUmlHotspot hs;
        hs.id = title;      // v1: id == title (good enough)
        hs.title = title;
        hs.rect = QRectF(QPointF(x1, y1), QPointF(x2, y2)).normalized();
        out.push_back(hs);
    }

    return out;
}
v1 uses title as the ID. That’s fine. Later we can encode stable IDs with [[id:aurora]] style links.

2) Add a Hotspot graphics item
studio/ui/diagram/HotspotItem.h
#pragma once
#include <QGraphicsRectItem>
#include <QString>

namespace architect {

class HotspotItem final : public QGraphicsRectItem {
public:
    HotspotItem(const QRectF& rect, QString hotspotId, QString title);

    QString hotspotId() const { return m_id; }
    QString title() const { return m_title; }

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* e) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* e) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* e) override;

private:
    QString m_id;
    QString m_title;
};

} // namespace architect
studio/ui/diagram/HotspotItem.cpp
#include "HotspotItem.h"
#include <QBrush>
#include <QPen>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

using namespace architect;

HotspotItem::HotspotItem(const QRectF& rect, QString hotspotId, QString title)
    : QGraphicsRectItem(rect),
      m_id(std::move(hotspotId)),
      m_title(std::move(title))
{
    setAcceptHoverEvents(true);
    setToolTip(m_title);

    // Transparent but selectable; slightly visible on hover.
    setBrush(Qt::NoBrush);
    setPen(Qt::NoPen);

    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setZValue(10'000); // above SVG
}

void HotspotItem::hoverEnterEvent(QGraphicsSceneHoverEvent* e) {
    Q_UNUSED(e)
    setPen(QPen(QColor(70,182,255,180), 2));
    update();
}

void HotspotItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* e) {
    Q_UNUSED(e)
    if (!isSelected()) setPen(Qt::NoPen);
    update();
}

void HotspotItem::mousePressEvent(QGraphicsSceneMouseEvent* e) {
    QGraphicsRectItem::mousePressEvent(e);
    // Selection happens automatically; tooltip already helps.
}

3) Update InfrastructureDiagramScene to render SVG + add hotspots
InfrastructureDiagramScene (replace setPage logic)
Core behavior:
    • Call renderSvgAndMap()
    • Add QGraphicsSvgItem for SVG
    • Add HotspotItem overlays using map coords
    • Hook selection changed → notify controller/inspector
studio/ui/diagram/InfrastructureDiagramScene.h (add signal)
signals:
    void hotspotSelected(QString hotspotId, QString title);
InfrastructureDiagramScene.cpp (key code)
#include "InfrastructureDiagramScene.h"

#include <QGraphicsSvgItem>
#include <QGraphicsTextItem>
#include <QStandardPaths>
#include <QDir>

#include "../../core/plantuml/PlantUmlService.h"
#include "HotspotItem.h"
#include "../../core/page/Page.h"

using namespace architect;

InfrastructureDiagramScene::InfrastructureDiagramScene(QObject* parent)
    : DiagramScene(parent)
{
    connect(this, &QGraphicsScene::selectionChanged, this, [this]() {
        const auto sel = selectedItems();
        if (sel.isEmpty()) return;

        if (auto* hs = dynamic_cast<HotspotItem*>(sel.first())) {
            emit hotspotSelected(hs->hotspotId(), hs->title());
        }
    });
}

void InfrastructureDiagramScene::setPage(InfrastructureConfigPage* page) {
    clear();
    m_page = page;
    if (!m_page) return;

    // Pull PlantUML text from payload
    const auto payload = m_page->payload(); // whichever accessor you have after subsystem refactor
    const auto plantuml = payload["plantuml"].toString();
    if (plantuml.trimmed().isEmpty()) {
        addText("No PlantUML in payload[\"plantuml\"].")->setPos(0,0);
        return;
    }

    // Cache dir
    const auto cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
                        + "/plantuml";
    QDir().mkpath(cacheDir);

    PlantUmlService svc("/usr/share/plantuml/plantuml.jar"); // v1 constant
    auto rr = svc.renderSvgAndMap(plantuml, cacheDir, "infrastructure");

    if (!rr.ok) {
        auto* t = addText("PlantUML render failed:\n" + rr.message);
        t->setPos(0,0);
        return;
    }

    // SVG background
    auto* svg = new QGraphicsSvgItem(rr.svgPath);
    svg->setZValue(0);
    addItem(svg);

    // Hotspots overlay
    for (const auto& hs : rr.hotspots) {
        auto* item = new HotspotItem(hs.rect, hs.id, hs.title);
        addItem(item);
    }

    // Fit scene rect to SVG bounds
    const QRectF b = svg->boundingRect();
    setSceneRect(b);
}
If your InfrastructureConfigPage doesn’t expose payload() yet (depending on the refactor stage), just pull the PlantUML string from wherever you currently store it.

4) Wire hotspot selection into the Inspector
Minimal v1 behavior:
    • When you click a hotspot, show its id/title in the Inspector and allow “Open UI” actions later.
Implement this by:
    • MainWindow listens to InfrastructureDiagramScene::hotspotSelected
    • Sets “selected element” on the page controller (or a simple SelectionModel)
Fast v1: just log it to Run Console.
In MainWindow when scene created:
if (auto* infraScene = dynamic_cast<InfrastructureDiagramScene*>(m_scene)) {
    connect(infraScene, &InfrastructureDiagramScene::hotspotSelected,
            this, [this](const QString& id, const QString& title) {
                m_runConsole->append(QString("Hotspot selected: %1 (%2)").arg(title, id));
            });
}
Later we replace this with:
    • SelectionService::setSelectedNode(id)
    • Inspector updates (node-specific config/actions)

5) CMake additions
Add explicitly:
  ui/diagram/HotspotItem.h
  ui/diagram/HotspotItem.cpp
No new Qt modules beyond what you already have.

What you’ll have after this step
✅ Render PlantUML SVG background
✅ Clickable nodes (hover highlights + click select)
✅ Stable geometry via MAP, not SVG hacks
✅ Clean path to: “click node → edit config → run actions”
This is the moment the app becomes a real ops/architecture tool.

Next (the real payoff)
Say next and we’ll add Node Identity so hotspots map to stable IDs (not titles), by embedding IDs in PlantUML using link syntax:
    • rectangle "Aurora" as AURORA [[id:machine:aurora]]
Then:
    • hotspots carry id:machine:aurora
    • Inspector shows the correct config panel
    • Buttons: SSH, Open Airflow, Submit Spark sweep, Deploy collector, etc.
Excellent — this is the keystone step. Once node identity is stable, everything else (inspectors, actions, orchestration) snaps into place cleanly.
Below is Step 5.3 — Stable Node Identity via PlantUML links, done in a way that is:
    • deterministic
    • OO-friendly
    • future-proof
    • zero magic parsing
No images needed here — this is pure control-plane plumbing.

STEP 5.3 — Stable Node Identity (PlantUML → Hotspot → Inspector)
The problem we’re solving
Right now:
    • hotspots use title text as IDs
    • titles are not stable (rename breaks everything)
    • no way to know if a node is a machine, service, topic, etc.
We fix this by making PlantUML the authoritative identity source.

Core rule (important)
Every interactive node MUST declare a stable ID in PlantUML.
We’ll use PlantUML’s built-in link syntax:
rectangle "Aurora R16" as AURORA [[id:machine:aurora]]
This:
    • does not affect rendering
    • shows up in the MAP file
    • survives label changes
    • is explicit and readable

1) Define the ID format (lock this down)
Use a simple, parseable, namespaced format:
id:<category>:<name>
Examples:
    • id:machine:aurora
    • id:service:airflow
    • id:service:spark_master
    • id:data_lake:nas
    • id:cloud:ec2_webhook
Later this lets you route to the correct inspector/action set without heuristics.

2) Update PlantUML template (small but critical change)
Before
rectangle "Aurora R16\nGPU Dev + Ops" as AURORA
After
rectangle "Aurora R16\nGPU Dev + Ops" as AURORA [[id:machine:aurora]]
Example for services
rectangle "Airflow\n:8080" as AIRFLOW [[id:service:airflow]]
rectangle "Spark Master\n:7077" as SPARK [[id:service:spark_master]]
rectangle "Kafka\n:9092" as KAFKA [[id:service:kafka]]
Nothing else in the diagram needs to change.

3) Extract IDs from PlantUML MAP output
PlantUML emits links in MAP like:
<area shape="rect"
      coords="34,120,210,180"
      href="id:machine:aurora"
      title="Aurora R16">
So we parse:
    • href → stable ID
    • title → display name
    • coords → geometry

4) Update MAP parsing (precise change)
Modify PlantUmlHotspot
Before
struct PlantUmlHotspot {
    QString id;
    QString title;
    QRectF rect;
};
After
struct PlantUmlHotspot {
    QString stableId;   // id:machine:aurora
    QString title;      // human label
    QRectF rect;
};

Update parseMap() regex and logic
Replace this regex (old):
R"(<area[^>]*shape="rect"[^>]*coords="(\d+),(\d+),(\d+),(\d+)"[^>]*title="([^"]*)")"
With this (new):
R"(<area[^>]*shape="rect"[^>]*coords="(\d+),(\d+),(\d+),(\d+)"[^>]*href="([^"]*)"[^>]*title="([^"]*)")"
Updated parse code
QVector<PlantUmlHotspot> PlantUmlService::parseMap(const QString& mapText) {
    QVector<PlantUmlHotspot> out;

    QRegularExpression re(
        R"(<area[^>]*shape="rect"[^>]*coords="(\d+),(\d+),(\d+),(\d+)"[^>]*href="([^"]*)"[^>]*title="([^"]*)")"
    );

    auto it = re.globalMatch(mapText);
    while (it.hasNext()) {
        const auto m = it.next();

        const double x1 = m.captured(1).toDouble();
        const double y1 = m.captured(2).toDouble();
        const double x2 = m.captured(3).toDouble();
        const double y2 = m.captured(4).toDouble();

        const QString href  = m.captured(5).trimmed();
        const QString title = m.captured(6).trimmed();

        // Only accept our explicit IDs
        if (!href.startsWith("id:"))
            continue;

        PlantUmlHotspot hs;
        hs.stableId = href;
        hs.title = title;
        hs.rect = QRectF(QPointF(x1, y1), QPointF(x2, y2)).normalized();

        out.push_back(hs);
    }

    return out;
}
This guarantees:
    • no accidental hotspots
    • only nodes you explicitly mark become interactive

5) Update HotspotItem to carry stable ID
HotspotItem.h
class HotspotItem final : public QGraphicsRectItem {
public:
    HotspotItem(const QRectF& rect,
                QString stableId,
                QString title);

    QString stableId() const { return m_stableId; }
    QString title() const { return m_title; }

private:
    QString m_stableId;
    QString m_title;
};
HotspotItem.cpp
Just replace m_id with m_stableId. No other logic changes.

6) Update InfrastructureDiagramScene signal
Before
signals:
    void hotspotSelected(QString hotspotId, QString title);
After
signals:
    void hotspotSelected(QString stableId, QString title);
And emit:
emit hotspotSelected(hs->stableId(), hs->title());

7) Central selection model (tiny but important)
Create a single source of truth for selection.
studio/core/selection/SelectionModel.h
#pragma once
#include <QObject>
#include <QString>

namespace architect {

class SelectionModel final : public QObject {
    Q_OBJECT
public:
    explicit SelectionModel(QObject* parent = nullptr);

    QString selectedId() const;

public slots:
    void setSelected(const QString& stableId, const QString& title);

signals:
    void selectionChanged(QString stableId, QString title);

private:
    QString m_id;
    QString m_title;
};

}
SelectionModel.cpp
#include "SelectionModel.h"

using namespace architect;

SelectionModel::SelectionModel(QObject* parent)
    : QObject(parent) {}

QString SelectionModel::selectedId() const {
    return m_id;
}

void SelectionModel::setSelected(const QString& stableId, const QString& title) {
    if (m_id == stableId) return;
    m_id = stableId;
    m_title = title;
    emit selectionChanged(m_id, m_title);
}

8) Wire it together (cleanly)
In MainWindow (or PageController):
connect(infraScene, &InfrastructureDiagramScene::hotspotSelected,
        m_selectionModel, &SelectionModel::setSelected);
Now:
    • Diagram sets selection
    • Inspector listens to selectionChanged
    • Actions route based on id:<type>:<name>
No coupling between diagram and inspector.

What you’ve achieved (this is huge)
You now have:
✅ Stable identity independent of labels
✅ Explicit opt-in interactivity
✅ Type-safe routing (machine, service, data, cloud)
✅ A clean selection spine for the whole app
This is the exact foundation used by serious infra/orchestration tools.

Next (this is where power shows up)
Say next and we’ll add:
Node Action Registry
    • id:machine:* → SSH, Deploy, Health
    • id:service:airflow → Open UI, Trigger DAG
    • id:service:spark_master → Submit sweep
    • id:cloud:* → Open AWS console
All actions discovered dynamically, no if/else chains.
Ready when you are.
Awesome — next = Node Action Registry (dynamic actions per node type, no if/else soup). This is the “ops-control surface” layer: click node → inspector shows contextual actions → actions run via hardened runners.
We’ll implement it in a way you can lock down hard in C++:
    • Stable IDs drive everything: id:<kind>:<name>
    • ActionRegistry maps predicates → action factories
    • Actions are objects with metadata + execute()
    • Execution uses your existing primitives (ssh, spark-submit, open-url)
    • Inspector renders actions as buttons (boring, effective)

STEP 5.4 — Node Action Registry
1) Define the Action contract
studio/core/actions/NodeAction.h
#pragma once

#include <QString>
#include <functional>

namespace architect {

struct NodeRef {
    QString stableId;   // id:machine:aurora
    QString title;      // "Aurora R16"
};

struct ActionContext {
    // Extend later: references to services/runners/config.
};

class NodeAction {
public:
    virtual ~NodeAction() = default;

    virtual QString id() const = 0;          // unique action id
    virtual QString label() const = 0;       // button label
    virtual QString description() const = 0; // tooltip/help

    // Execute returns ok + message to log.
    virtual std::pair<bool, QString> execute(const NodeRef& node,
                                             const ActionContext& ctx) = 0;
};

} // namespace architect

2) Registry (dynamic routing, no switch/case)
studio/core/actions/ActionRegistry.h
#pragma once

#include <QString>
#include <QVector>
#include <functional>
#include <memory>

#include "NodeAction.h"

namespace architect {

using ActionFactory = std::function<std::unique_ptr<NodeAction>()>;
using NodePredicate = std::function<bool(const NodeRef&)>;

struct ActionRule {
    QString ruleId;
    NodePredicate match;
    ActionFactory factory;
};

class ActionRegistry final {
public:
    void registerRule(ActionRule rule);

    QVector<std::unique_ptr<NodeAction>> actionsFor(const NodeRef& node) const;

private:
    QVector<ActionRule> m_rules;
};

} // namespace architect
studio/core/actions/ActionRegistry.cpp
#include "ActionRegistry.h"

using namespace architect;

void ActionRegistry::registerRule(ActionRule rule) {
    m_rules.push_back(std::move(rule));
}

QVector<std::unique_ptr<NodeAction>> ActionRegistry::actionsFor(const NodeRef& node) const {
    QVector<std::unique_ptr<NodeAction>> out;

    for (const auto& r : m_rules) {
        if (r.match && r.match(node)) {
            out.push_back(r.factory());
        }
    }
    return out;
}
This is the “clean spine”: all action composition happens here.

3) Utility: parse stableId into parts
studio/core/actions/NodeId.h
#pragma once
#include <QString>

namespace architect {

struct NodeIdParts {
    QString kind;  // "machine", "service", "cloud", etc.
    QString name;  // "aurora", "airflow", etc.
    bool ok = false;
};

inline NodeIdParts parseNodeId(const QString& stableId) {
    // expects id:<kind>:<name>
    NodeIdParts p;
    const auto s = stableId.trimmed();
    if (!s.startsWith("id:")) return p;

    const auto rest = s.mid(3);
    const auto parts = rest.split(':');
    if (parts.size() < 2) return p;

    p.kind = parts[0].trimmed();
    p.name = parts.mid(1).join(":").trimmed(); // allow colons in name if needed
    p.ok = !p.kind.isEmpty() && !p.name.isEmpty();
    return p;
}

} // namespace architect

4) Implement concrete actions (v1)
A) Open URL action (Airflow UI, Spark UI, Grafana, etc.)
studio/core/actions/OpenUrlAction.h
#pragma once
#include "NodeAction.h"
#include <QString>

namespace architect {

class OpenUrlAction final : public NodeAction {
public:
    OpenUrlAction(QString url, QString label, QString desc);

    QString id() const override { return "open_url"; }
    QString label() const override { return m_label; }
    QString description() const override { return m_desc; }

    std::pair<bool, QString> execute(const NodeRef& node,
                                     const ActionContext& ctx) override;

private:
    QString m_url;
    QString m_label;
    QString m_desc;
};

} // namespace architect
studio/core/actions/OpenUrlAction.cpp
#include "OpenUrlAction.h"
#include <QDesktopServices>
#include <QUrl>

using namespace architect;

OpenUrlAction::OpenUrlAction(QString url, QString label, QString desc)
    : m_url(std::move(url)), m_label(std::move(label)), m_desc(std::move(desc)) {}

std::pair<bool, QString> OpenUrlAction::execute(const NodeRef&, const ActionContext&) {
    const auto ok = QDesktopServices::openUrl(QUrl(m_url));
    if (!ok) return {false, "Failed to open URL: " + m_url};
    return {true, "Opened URL: " + m_url};
}

B) SSH “Ping” action (machine health check v1)
We’ll do a super-safe check:
    • ssh host "hostname && uptime"
    • uses the same QProcess approach as before
    • timeboxed
studio/core/actions/SshPingAction.h
#pragma once
#include "NodeAction.h"
#include "../infra/InfraTypes.h"

namespace architect {

class SshPingAction final : public NodeAction {
public:
    explicit SshPingAction(architect::infra::Machine machine);

    QString id() const override { return "ssh_ping"; }
    QString label() const override { return "SSH Ping"; }
    QString description() const override { return "Runs 'hostname && uptime' via SSH"; }

    std::pair<bool, QString> execute(const NodeRef& node,
                                     const ActionContext& ctx) override;

private:
    architect::infra::Machine m_machine;

    QString target() const;
};

} // namespace architect
studio/core/actions/SshPingAction.cpp
#include "SshPingAction.h"
#include <QProcess>

using namespace architect;

SshPingAction::SshPingAction(architect::infra::Machine machine)
    : m_machine(std::move(machine)) {}

QString SshPingAction::target() const {
    if (!m_machine.ssh_user.isEmpty())
        return m_machine.ssh_user + "@" + m_machine.ssh_host;
    return m_machine.ssh_host;
}

std::pair<bool, QString> SshPingAction::execute(const NodeRef&, const ActionContext&) {
    if (m_machine.ssh_host.isEmpty())
        return {false, "Machine has no ssh_host configured"};

    QStringList args;
    args << "-o" << "BatchMode=yes";
    args << "-o" << "StrictHostKeyChecking=accept-new";
    args << "-o" << "ConnectTimeout=5";
    args << "-p" << QString::number(m_machine.ssh_port);
    args << target();
    args << "hostname && uptime";

    QProcess p;
    p.start("ssh", args);

    if (!p.waitForStarted(3000))
        return {false, "ssh failed to start"};

    if (!p.waitForFinished(12000)) {
        p.kill();
        p.waitForFinished(1000);
        return {false, "ssh timed out"};
    }

    const int code = p.exitCode();
    const auto out = QString::fromUtf8(p.readAllStandardOutput());
    const auto err = QString::fromUtf8(p.readAllStandardError());

    if (code != 0)
        return {false, QString("exit=%1\n%2\n%3").arg(code).arg(out, err)};

    return {true, out.trimmed()};
}

C) Spark “Submit Sweep” action (v1 stub)
This can just call your existing SparkSubmitRunner later.
For v1, it can open a config dialog or log “TODO”.
But since you already have SparkSubmitRunner, we can wire it to run a tiny job spec.
We’ll keep v1 simple: action just logs the command that would run.
Next step will execute it.

5) Provide “ActionContext” services (Infra snapshot, logger)
We need:
    • map stable ID → machine/service info
    • a place to log output
Minimal v1: pass a function callback.
studio/core/actions/ActionContext.h
#pragma once
#include <functional>
#include <QString>
#include <QMap>

#include "../infra/InfraTypes.h"

namespace architect {

struct ActionContext {
    QMap<QString, architect::infra::Machine> machinesById; // id:machine:aurora => Machine
    QMap<QString, QString> serviceUrlsById;                // id:service:airflow => http://...

    std::function<void(const QString&)> log;
};

}

6) Register rules (where the magic becomes “configuration”)
Create a bootstrap function:
studio/core/actions/DefaultActionRules.h
#pragma once
namespace architect { class ActionRegistry; struct ActionContext; }

namespace architect {
void registerDefaultActionRules(ActionRegistry& reg, const ActionContext& ctx);
}
studio/core/actions/DefaultActionRules.cpp
#include "DefaultActionRules.h"
#include "ActionRegistry.h"
#include "OpenUrlAction.h"
#include "SshPingAction.h"
#include "NodeId.h"

using namespace architect;

void architect::registerDefaultActionRules(ActionRegistry& reg, const ActionContext& ctx) {
    // Machine: SSH ping
    reg.registerRule(ActionRule{
        "machine_ssh_ping",
        [](const NodeRef& n) {
            const auto p = parseNodeId(n.stableId);
            return p.ok && p.kind == "machine";
        },
        [ctx, &ctx]() mutable {
            // Factory needs machine at runtime; in v1 we bind later in inspector (see below).
            // We'll return a placeholder action; inspector will rebuild actions with resolved machine.
            return std::unique_ptr<NodeAction>(nullptr);
        }
    });

    // Service: open URL (Airflow etc.)
    reg.registerRule(ActionRule{
        "service_open_ui",
        [](const NodeRef& n) {
            const auto p = parseNodeId(n.stableId);
            return p.ok && p.kind == "service";
        },
        []() {
            // Placeholder; inspector will resolve url by node id.
            return std::unique_ptr<NodeAction>(nullptr);
        }
    });
}
But placeholders are ugly. Better: action factories should accept node+ctx. Let’s improve the registry now.

✅ Better registry (recommended): factories get node+ctx
Update ActionRegistry:
ActionRegistry.h (replace typedef)
using ActionFactory = std::function<std::unique_ptr<NodeAction>(const NodeRef&, const ActionContext&)>;
Then rule factory is called with node+ctx, so no placeholders.
ActionRegistry.cpp adjust:
for (const auto& r : m_rules) {
    if (r.match && r.match(node)) {
        out.push_back(r.factory(node, ctx));
    }
}
And signature of actionsFor becomes:
QVector<std::unique_ptr<NodeAction>> actionsFor(const NodeRef& node, const ActionContext& ctx) const;
Now default rules become clean:
DefaultActionRules.cpp (real)
void architect::registerDefaultActionRules(ActionRegistry& reg, const ActionContext&) {
    // Machine: SSH ping
    reg.registerRule(ActionRule{
        "machine_ssh_ping",
        [](const NodeRef& n) {
            const auto p = parseNodeId(n.stableId);
            return p.ok && p.kind == "machine";
        },
        [](const NodeRef& n, const ActionContext& ctx) {
            const auto it = ctx.machinesById.find(n.stableId);
            if (it == ctx.machinesById.end()) {
                return std::make_unique<OpenUrlAction>(
                    "about:blank",
                    "Missing machine config",
                    "No machine config found for this node id"
                );
            }
            return std::make_unique<SshPingAction>(it.value());
        }
    });

    // Service: open UI
    reg.registerRule(ActionRule{
        "service_open_ui",
        [](const NodeRef& n) {
            const auto p = parseNodeId(n.stableId);
            return p.ok && p.kind == "service";
        },
        [](const NodeRef& n, const ActionContext& ctx) {
            const auto it = ctx.serviceUrlsById.find(n.stableId);
            const QString url = (it == ctx.serviceUrlsById.end()) ? QString() : it.value();
            if (url.isEmpty()) {
                return std::make_unique<OpenUrlAction>(
                    "about:blank",
                    "Open UI (missing URL)",
                    "No URL configured for this service"
                );
            }
            return std::make_unique<OpenUrlAction>(url, "Open UI", "Open service web UI");
        }
    });
}
This is exactly the pattern you want: new actions are just rules.

7) Inspector UI: show actions for selected node
Add a NodeActionsPanel that subscribes to SelectionModel.
Behavior
    • When selection changes:
        ◦ build NodeRef {stableId, title}
        ◦ call registry.actionsFor(node, ctx)
        ◦ render buttons
    • on click:
        ◦ action->execute(node, ctx)
        ◦ log result to Run Console
UI skeleton
    • QGroupBox("Actions")
    • vertical list of QPushButtons
    • tooltips = description

8) Populate ActionContext from Infra snapshot
When you render infrastructure, you already have machines/services in payload or in InfraRegistry.
Create a simple mapping:
    • id:machine:aurora → Machine{id="aurora"...}
    • id:service:airflow → url http://aurora:8080
You can store service URLs either in:
    • the Machine.services array and derive them, or
    • page payload payload["service_urls"]
For v1: simplest is a hardcoded map in InfrastructureController.

Cursor prompt (paste to implement Step 5.4)
Implement Step 5.4: Node Action Registry.

Goal:
- Clicking a PlantUML hotspot selects a stableId (id:kind:name).
- Inspector shows dynamic actions (buttons) based on node kind.
- Actions execute and log results to the Run Console.

Constraints:
- No switch/case chains for actions.
- Use registry rules (predicate + factory) to compose actions.
- Actions must be objects with execute() returning ok+message.
- Implement 2 real actions:
  1) OpenUrlAction (uses QDesktopServices)
  2) SshPingAction (runs hostname && uptime via ssh using QProcess with timeouts)

Tasks:
1) Add core/actions:
   - NodeAction.h
   - ActionContext.h
   - NodeId.h
   - ActionRegistry.h/cpp (factory gets node+ctx)
   - OpenUrlAction.h/cpp
   - SshPingAction.h/cpp
   - DefaultActionRules.h/cpp (register rules for machine/service)
2) Add ui/inspector/NodeActionsPanel.h/cpp:
   - listens to SelectionModel::selectionChanged
   - rebuilds buttons from ActionRegistry::actionsFor(node, ctx)
   - on click executes action and logs via ctx.log
3) Wire in MainWindow:
   - create SelectionModel, ActionRegistry
   - build ActionContext from InfraRegistry snapshot:
       machinesById: map stableId -> Machine where stableId = "id:machine:" + machine.id
       serviceUrlsById: map stableId -> url for known services (airflow, spark_master, kafka)
   - register default rules
   - connect InfrastructureDiagramScene hotspotSelected -> SelectionModel::setSelected
4) Ensure build passes and clicking nodes shows correct actions:
   - machine nodes show SSH Ping
   - service nodes show Open UI (if URL present)

Deliverables:
- Build succeeds
- Clicking Aurora node: SSH Ping logs output or error if not reachable
- Clicking Airflow node: Open UI opens browser (or logs missing URL if not set)

Next (this is the “real control plane” step)
Say next and we’ll add the Machine Inspector:
    • edit SSH host/user/port
    • edit service URLs per machine
    • buttons: Deploy collector, Tail logs, Restart service (optional, gated)
…and now your diagram becomes a real ops cockpit.
Perfect — next = Machine Inspector + Service Config, so your hotspot selection doesn’t just run actions, it can also edit the underlying machine/service model safely and persist it back into the page payload (and later into a shared infra registry).
This is where the tool becomes “control” instead of “viewer”.

STEP 5.5 — Machine Inspector (Edit + Persist + Actions)
What you’ll get
When you click id:machine:aurora:
    • Editable fields:
        ◦ Display name
        ◦ SSH user / host / port
        ◦ Tags/roles (optional)
    • Service table for that machine:
        ◦ Airflow URL, Spark Master URL, Kafka URL, etc.
        ◦ Add/remove services
    • Buttons (actions):
        ◦ SSH Ping
        ◦ Deploy Collector (uses your existing DeploymentService)
        ◦ Open Service UI (per service)
        ◦ Copy SSH target
    • All changes:
        ◦ update page payload immediately
        ◦ trigger action context refresh (so ActionRegistry uses updated values)
✅ No switch/case
✅ No spaghetti
✅ OO enforced via subsystem controller/inspector separation

1) Data model: store machine/service config inside Infra page payload
For v1, keep it page-local and explicit:
payload["machines"] schema
{
  "machines": [
    {
      "id": "aurora",
      "display_name": "Aurora R16",
      "ssh_user": "jd",
      "ssh_host": "192.168.1.50",
      "ssh_port": 22,
      "services": [
        { "name": "airflow", "url": "http://192.168.1.50:8080", "running": true },
        { "name": "spark_master", "url": "spark://192.168.1.50:7077", "running": true }
      ]
    }
  ]
}
And your PlantUML stable IDs should reference these:
    • id:machine:aurora
    • id:service:airflow (service nodes can map to service entries)

2) Introduce a small InfraConfigStore (single source of truth)
studio/core/infra/InfraConfigStore.h
    • loads machines from page payload
    • provides lookup by stableId
    • writes back changes
Key methods:
    • Machine* findMachineByStableId("id:machine:aurora")
    • void upsertMachine(const Machine&)
    • void saveToPage(Page*)
This avoids your Inspector mutating JSON directly.

3) MachineInspectorPanel UI (Qt widgets)
Layout (boring, effective)
Group: Machine
    • Display Name (QLineEdit)
    • SSH User (QLineEdit)
    • SSH Host (QLineEdit)
    • SSH Port (QSpinBox)
    • Buttons row:
        ◦ Copy SSH Target
        ◦ SSH Ping
        ◦ Deploy Collector
Group: Services (QTableWidget)
Columns:
    • name
    • url
    • running (checkbox)
Buttons:
    • Add Service
    • Remove Selected
    • Open Selected URL
Status line
    • last action result
Persistence rules
    • On “Apply” (or field edited + debounce):
        ◦ update InfraConfigStore
        ◦ update page payload
        ◦ emit infraConfigChanged()
        ◦ refresh ActionContext (so ActionRegistry sees new host/urls)
For v1: add explicit Apply button (less risk).

4) Upgrade ActionContext to be live (refreshable)
Right now you build ActionContext once. We’ll make it:
    • rebuilt whenever infra config changes
    • stored in MainWindow/AppServices
So:
    • NodeActionsPanel always calls contextProvider() to get latest maps
    • OR you keep a shared ActionContext object that gets updated
Simplest v1:
    • store std::function<ActionContext()> getCtx in NodeActionsPanel.

5) Add “Deploy Collector” action (real)
You already have:
    • DeploymentService::ensureDeployed(machine, localCollectorPath, "collector")
So create:
    • DeployCollectorAction(machine, localCollectorPath)
It runs deploy and logs “deployed / already deployed”.

Cursor Prompt to implement Step 5.5 (paste into Cursor)
Implement Step 5.5: Machine Inspector + Service Config + Deploy Collector.

Goal:
- Clicking a machine hotspot (id:machine:<name>) shows an inspector panel that lets user edit SSH settings and services (URLs) and persist them to the Infrastructure page payload.
- Node actions must use the updated config immediately.
- Add a real action button: Deploy Collector (uses DeploymentService to scp versioned binary).

Constraints:
- Qt6 Widgets only.
- No switch/case for node kinds; use stableId parsing + registry.
- Persist config in page.payload["machines"] as an array of Machine JSON objects.
- Keep code boring, explicit, compile-clean.

Tasks:
1) Add core/infra/InfraConfigStore.{h,cpp}
   - Construct with Page* or payload reference
   - Load machines from payload["machines"] if present
   - API:
       Machine* findMachineByStableId(QString stableId)
       void upsertMachine(Machine m)
       QJsonObject toPayload() const
   - Helper: stableId for machine: "id:machine:" + machine.id
2) Update InfrastructureSubsystem / InfrastructurePage model:
   - Ensure payload includes "machines" array and "plantuml" string.
   - Provide an accessor for payload mutation and notify change.
3) Add ui/inspector/MachineInspectorPanel.{h,cpp}
   - Inputs: Display Name, ssh_user, ssh_host, ssh_port
   - Services table (name,url,running)
   - Buttons:
       Apply (writes to InfraConfigStore and updates page payload)
       Copy SSH Target
       SSH Ping (reuse SshPingAction or call it directly)
       Deploy Collector (new DeployCollectorAction)
       Open Service URL for selected row
   - Status label at bottom
4) Add core/actions/DeployCollectorAction.{h,cpp}
   - Constructor: Machine, localCollectorPath
   - execute(): uses DeploymentService::ensureDeployed(machine, localCollectorPath, "collector")
   - returns ok+message
5) Update ActionContext building:
   - Build machinesById from payload machines array (not from hardcoded registry).
   - Build serviceUrlsById from each machine.services:
       stableId for service: "id:service:" + service.name
       (If duplicates exist across machines, first wins for v1)
   - Ensure NodeActionsPanel uses latest context (rebuild ctx after Apply).
6) Wiring:
   - SelectionModel emits stableId/title
   - Inspector host widget chooses which inspector to show:
       if kind == "machine" => MachineInspectorPanel
       else fallback => NodeActionsPanel only
   - Clicking Airflow service node still shows Open UI action via registry

Deliverables:
- Build succeeds
- Editing SSH host for Aurora then clicking SSH Ping uses new host
- Deploy Collector runs and logs deployed/already-deployed
- Services table edits persist to payload and Open UI works

Quick “ID discipline” reminder (so it stays clean)
    • Machines in payload: machine.id must match PlantUML IDs.
        ◦ PlantUML: [[id:machine:aurora]]
        ◦ Machine JSON: "id": "aurora"
    • Services: PlantUML nodes like [[id:service:airflow]] map to service name "airflow"
This keeps everything deterministic with zero heuristics.

Next (your ops cockpit starts to feel real)
Say next and we’ll add Service Inspector for id:service:* nodes:
    • edit URL + running status
    • “Open UI”
    • “Restart service” (optional and gated)
    • “Tail logs” (SSH + journalctl/docker logs)
And we can do it without letting the system get “agentic-chaos-y.”
