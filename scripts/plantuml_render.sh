#!/usr/bin/env bash
#
# plantuml_render.sh - Render PlantUML (.puml) to SVG
#
# Usage: ./plantuml_render.sh <input.puml> [output.svg]
#
# Requires: java, plantuml.jar installed on system
#
set -euo pipefail

# Default PlantUML jar locations (adjust if needed)
PLANTUML_JAR="${PLANTUML_JAR:-/usr/share/plantuml/plantuml.jar}"

# Alternative common locations
if [[ ! -f "$PLANTUML_JAR" ]]; then
    for candidate in \
        /usr/share/java/plantuml.jar \
        /opt/plantuml/plantuml.jar \
        ~/plantuml.jar \
        ./plantuml.jar; do
        if [[ -f "$candidate" ]]; then
            PLANTUML_JAR="$candidate"
            break
        fi
    done
fi

# Check arguments
if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <input.puml> [output.svg]"
    echo ""
    echo "Environment:"
    echo "  PLANTUML_JAR - Path to plantuml.jar (default: /usr/share/plantuml/plantuml.jar)"
    exit 1
fi

INPUT_FILE="$1"
OUTPUT_FILE="${2:-}"

# Validate input
if [[ ! -f "$INPUT_FILE" ]]; then
    echo "ERROR: Input file not found: $INPUT_FILE"
    exit 1
fi

# Validate PlantUML jar
if [[ ! -f "$PLANTUML_JAR" ]]; then
    echo "ERROR: plantuml.jar not found at: $PLANTUML_JAR"
    echo ""
    echo "Install PlantUML or set PLANTUML_JAR environment variable."
    echo "  Ubuntu: sudo apt install plantuml"
    exit 1
fi

# Determine output path
if [[ -z "$OUTPUT_FILE" ]]; then
    # Default: same directory, .svg extension
    OUTPUT_FILE="${INPUT_FILE%.puml}.svg"
fi

OUTPUT_DIR="$(dirname "$OUTPUT_FILE")"
mkdir -p "$OUTPUT_DIR"

echo "Rendering: $INPUT_FILE -> $OUTPUT_FILE"

# Render to SVG
java -jar "$PLANTUML_JAR" -tsvg -o "$OUTPUT_DIR" "$INPUT_FILE"

# PlantUML outputs to same dir as input with .svg extension
# Move if output location differs
GENERATED="${INPUT_FILE%.puml}.svg"
if [[ "$GENERATED" != "$OUTPUT_FILE" && -f "$GENERATED" ]]; then
    mv "$GENERATED" "$OUTPUT_FILE"
fi

if [[ -f "$OUTPUT_FILE" ]]; then
    echo "Done: $OUTPUT_FILE"
else
    echo "ERROR: Failed to generate SVG"
    exit 1
fi

