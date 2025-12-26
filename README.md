# Strategient Architect

Qt6 C++ desktop application with a Rust engine for infrastructure orchestration and visualization.

## Prerequisites

- **Ubuntu 22.04+** (or compatible Linux)
- **Rust** (stable, via rustup)
- **Qt6** development packages
- **CMake** 3.20+
- **PlantUML** (optional, for diagram rendering)

### Install dependencies (Ubuntu)

```bash
# Build essentials
sudo apt update
sudo apt install build-essential cmake

# Qt6
sudo apt install qt6-base-dev qt6-svg-dev libqt6svg6-dev libqt6svgwidgets6

# Rust (if not installed)
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
source ~/.cargo/env

# PlantUML (optional)
sudo apt install plantuml
```

## Build

```bash
# Build everything (Rust engine + Qt studio)
./scripts/build_all.sh
```

## Run

```bash
./build/StrategientArchitect
```

The application will display the engine version in the Run Console on startup.

## PlantUML Rendering

Render PlantUML diagrams to SVG:

```bash
# Render sample diagram
./scripts/plantuml_render.sh assets/plantuml/sample_infra.puml /tmp/sample_infra.svg

# Then open the SVG in the app via File → Open SVG
```

## Project Structure

```
strategient-architect/
├── engine/              # Rust engine crate (staticlib)
│   ├── Cargo.toml
│   ├── src/lib.rs
│   └── include/engine.h # C ABI header
├── studio/              # Qt6 C++ application
│   ├── CMakeLists.txt
│   └── src/
├── scripts/             # Build and utility scripts
├── assets/              # PlantUML diagrams, etc.
└── docs/                # Documentation
```

## License

Proprietary - All rights reserved.

