#!/usr/bin/env bash
# Build script for esp-hub75
# 1. Build and run unit tests (GTest)
# 2. Build the ESP-IDF firmware

set -e

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
TEST_DIR="$PROJECT_DIR/test"
TEST_BUILD_DIR="$TEST_DIR/build"

# ── Unit Tests (GTest) ──────────────────────────────────────────────

echo "========================================="
echo " Building and running unit tests (GTest)"
echo "========================================="

cmake -S "$TEST_DIR" -B "$TEST_BUILD_DIR"
cmake --build "$TEST_BUILD_DIR"
ctest --test-dir "$TEST_BUILD_DIR" --output-on-failure

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

idf.py -C "$PROJECT_DIR" -DAPP="$APP" build

echo ""
echo "========================================="
echo " Build complete"
echo "========================================="
