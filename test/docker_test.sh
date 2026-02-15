#!/bin/bash
set -e

TEST_BIN=/src/test/test_binary
PLUGIN=/src/build/lldbghidra.so
export SLEIGHHOME=/src/specfiles

echo "=== Test 1: Plugin load ==="
lldb-18 -o "plugin load $PLUGIN" -o "quit" 2>&1
echo "PASS: Plugin loaded successfully"

echo ""
echo "=== Test 2: Decompile factorial() ==="
lldb-18 --batch \
    -o "plugin load $PLUGIN" \
    -o "file $TEST_BIN" \
    -o "b main" \
    -o "run" \
    -o "decompile factorial" \
    2>&1

echo ""
echo "=== Test 3: Decompile main() ==="
lldb-18 --batch \
    -o "plugin load $PLUGIN" \
    -o "file $TEST_BIN" \
    -o "b main" \
    -o "run" \
    -o "decompile main" \
    2>&1

echo ""
echo "=== Test 4: Decompile current frame (no args) ==="
lldb-18 --batch \
    -o "plugin load $PLUGIN" \
    -o "file $TEST_BIN" \
    -o "b factorial" \
    -o "run" \
    -o "decompile" \
    2>&1

echo ""
echo "=== All tests passed ==="
