#!/usr/bin/env bash
# Build and run an SDL example
# Usage: ./run_example.sh [example_name]
# Example: ./run_example.sh blank_red

set -e

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
EXAMPLE_DIR="$PROJECT_DIR/example/sdl"
BUILD_DIR="$EXAMPLE_DIR/build"

EXAMPLE="${1:-blank_red}"

# ── Build ──────────────────────────────────────────────────────────

echo "========================================="
echo " Building SDL examples"
echo "========================================="

cmake -S "$EXAMPLE_DIR" -B "$BUILD_DIR"
cmake --build "$BUILD_DIR"

# ── Run ────────────────────────────────────────────────────────────

BINARY="$BUILD_DIR/$EXAMPLE"

if [ ! -f "$BINARY" ]; then
    echo "Error: '$EXAMPLE' not found. Available examples:"
    find "$BUILD_DIR" -maxdepth 1 -type f -executable -printf "  %f\n"
    exit 1
fi

echo ""
echo "========================================="
echo " Running: $EXAMPLE"
echo "========================================="

"$BINARY"
