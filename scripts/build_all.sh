#!/usr/bin/env bash
#
# build_all.sh - Build Rust engine and Qt6 studio application
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_ROOT}/build"

echo "=== Strategient Architect Build ==="
echo "Project root: ${PROJECT_ROOT}"

# Step 1: Build Rust engine
echo ""
echo ">>> Building Rust engine..."
cd "${PROJECT_ROOT}/engine"
cargo build --release

if [[ ! -f "target/release/libengine.a" ]]; then
    echo "ERROR: Rust build failed - libengine.a not found"
    exit 1
fi
echo ">>> Rust engine built: target/release/libengine.a"

# Step 2: Build Qt6 studio
echo ""
echo ">>> Building Qt6 studio..."
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

cmake "${PROJECT_ROOT}/studio" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build . --parallel "$(nproc)"

if [[ ! -f "${BUILD_DIR}/StrategientArchitect" ]]; then
    echo "ERROR: Qt build failed - StrategientArchitect executable not found"
    exit 1
fi

echo ""
echo "=== Build Complete ==="
echo "Executable: ${BUILD_DIR}/StrategientArchitect"
echo ""
echo "Run with: ${BUILD_DIR}/StrategientArchitect"

