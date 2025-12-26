#!/usr/bin/env bash
#
# plantuml_render.sh - Render PlantUML (.puml) to SVG
#
# Usage: ./plantuml_render.sh <input.puml> [output.svg]
#
# Requires: java, plantuml installed (or plantuml.jar)
#
set -euo pipefail

# Check arguments
if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <input.puml> [output.svg]"
    echo ""
    echo "Examples:"
    echo "  $0 diagram.puml                  # outputs diagram.svg in same dir"
    echo "  $0 diagram.puml /tmp/out.svg     # outputs to specified path"
    exit 1
fi

INPUT_FILE="$1"
OUTPUT_FILE="${2:-}"

# Validate input
if [[ ! -f "$INPUT_FILE" ]]; then
    echo "ERROR: Input file not found: $INPUT_FILE"
    exit 1
fi

# Get absolute paths
INPUT_FILE="$(realpath "$INPUT_FILE")"
INPUT_DIR="$(dirname "$INPUT_FILE")"
INPUT_NAME="$(basename "$INPUT_FILE" .puml)"

# Determine output path
if [[ -z "$OUTPUT_FILE" ]]; then
    OUTPUT_FILE="${INPUT_DIR}/${INPUT_NAME}.svg"
else
    OUTPUT_FILE="$(realpath -m "$OUTPUT_FILE")"
fi

OUTPUT_DIR="$(dirname "$OUTPUT_FILE")"
mkdir -p "$OUTPUT_DIR"

echo "Rendering: $INPUT_FILE -> $OUTPUT_FILE"

# Check if plantuml command exists (Ubuntu package)
if command -v plantuml &> /dev/null; then
    # Use system plantuml command - render to temp, then move
    TEMP_DIR=$(mktemp -d)
    plantuml -tsvg "$INPUT_FILE" -o "$TEMP_DIR"
    
    GENERATED="${TEMP_DIR}/${INPUT_NAME}.svg"
    if [[ -f "$GENERATED" ]]; then
        mv "$GENERATED" "$OUTPUT_FILE"
        rm -rf "$TEMP_DIR"
    else
        rm -rf "$TEMP_DIR"
        echo "ERROR: Failed to generate SVG"
        exit 1
    fi
else
    # Try to find plantuml.jar
    PLANTUML_JAR="${PLANTUML_JAR:-/usr/share/plantuml/plantuml.jar}"
    
    for candidate in \
        /usr/share/java/plantuml.jar \
        /usr/share/plantuml/plantuml.jar \
        /opt/plantuml/plantuml.jar \
        ~/plantuml.jar \
        ./plantuml.jar; do
        if [[ -f "$candidate" ]]; then
            PLANTUML_JAR="$candidate"
            break
        fi
    done
    
    if [[ ! -f "$PLANTUML_JAR" ]]; then
        echo "ERROR: plantuml not found. Install with: sudo apt install plantuml"
        exit 1
    fi
    
    # Render using jar
    TEMP_DIR=$(mktemp -d)
    java -jar "$PLANTUML_JAR" -tsvg "$INPUT_FILE" -o "$TEMP_DIR"
    
    GENERATED="${TEMP_DIR}/${INPUT_NAME}.svg"
    if [[ -f "$GENERATED" ]]; then
        mv "$GENERATED" "$OUTPUT_FILE"
        rm -rf "$TEMP_DIR"
    else
        rm -rf "$TEMP_DIR"
        echo "ERROR: Failed to generate SVG"
        exit 1
    fi
fi

if [[ -f "$OUTPUT_FILE" ]]; then
    echo "Done: $OUTPUT_FILE"
else
    echo "ERROR: Output file not created"
    exit 1
fi
