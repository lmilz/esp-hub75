#!/usr/bin/env bash
# Build script for esp-hub75
# 1. Build and run unit tests (GTest)
# 2. Build the ESP-IDF firmware

set -e

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
TEST_DIR="$PROJECT_DIR/test"
TEST_BUILD_DIR="$TEST_DIR/build"

# ── Formatting ─────────────────────────────────────────────────────

echo "========================================="
echo " Running clang-format"
echo "========================================="

find "$PROJECT_DIR/include" "$PROJECT_DIR/test" "$PROJECT_DIR/examples" \
    \( -name '*.hpp' -o -name '*.cpp' \) | \
    grep -v '/build/' | \
    xargs clang-format -i --style=file

echo " Done."

# ── Unit Tests (GTest) ──────────────────────────────────────────────

echo "========================================="
echo " Building and running unit tests (GTest)"
echo "========================================="

cmake -S "$TEST_DIR" -B "$TEST_BUILD_DIR"
cmake --build "$TEST_BUILD_DIR"
ctest --test-dir "$TEST_BUILD_DIR" --output-on-failure

# Symlink compile_commands.json to project root (for clang-tidy / LSP)
ln -sf "$TEST_BUILD_DIR/compile_commands.json" "$PROJECT_DIR/compile_commands.json"

echo ""
echo "========================================="
echo " Unit tests passed"
echo "========================================="

# ── ESP-IDF Firmware Build ───────────────────────────────────────────

echo ""
echo "========================================="
echo " Building ESP-IDF firmware"
echo "========================================="

APP="${APP:-blank}"
echo "Selected app: $APP"

idf.py -C "$PROJECT_DIR/examples/esp32/$APP" build

echo ""
echo "========================================="
echo " Build complete"
echo "========================================="
