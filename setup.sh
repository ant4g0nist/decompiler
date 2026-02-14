#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
DECOM_DIR="$(dirname "$SCRIPT_DIR")"

GHIDRA_CPP="${DECOM_DIR}/ghidra/Ghidra/Features/Decompiler/src/decompile/cpp"
DEST="${SCRIPT_DIR}/third_party/ghidra-decompiler"
PATCHES_DIR="${DECOM_DIR}/r2ghidra/subprojects/packagefiles/ghidra-native/patches"

# ==============================
# Step 1: Copy Ghidra decompiler source
# ==============================
echo "=== Step 1: Copy Ghidra decompiler source ==="

if [ ! -d "$GHIDRA_CPP" ]; then
    echo "ERROR: Ghidra source not found at $GHIDRA_CPP"
    exit 1
fi

if [ ! -f "${DEST}/sleigh_arch.hh" ]; then
    echo "Copying decompiler source..."
    mkdir -p "$DEST"
    cp "$GHIDRA_CPP"/*.cc "$DEST/" 2>/dev/null || true
    cp "$GHIDRA_CPP"/*.hh "$DEST/" 2>/dev/null || true
    cp "$GHIDRA_CPP"/*.h "$DEST/" 2>/dev/null || true
    echo "Done."
else
    echo "Source files already present, skipping copy."
fi

# ==============================
# Step 2: Try to apply r2ghidra patches
# ==============================
echo ""
echo "=== Step 2: Apply r2ghidra patches ==="

if [ -d "$PATCHES_DIR" ]; then
    cd "$DEST"
    for patch in "$PATCHES_DIR"/*.patch; do
        pname=$(basename "$patch")
        echo -n "  $pname ... "
        if patch -p3 --forward --dry-run < "$patch" > /dev/null 2>&1; then
            patch -p3 --forward < "$patch" > /dev/null 2>&1
            echo "OK"
        else
            echo "SKIP"
        fi
    done
    cd "$SCRIPT_DIR"
else
    echo "No patches directory found."
fi

# ==============================
# Step 3: Apply critical patches manually (if not already applied)
# ==============================
echo ""
echo "=== Step 3: Manual critical patches ==="

# Patch: Make collectSpecFiles public + add getLanguageDescriptions
SLEIGH_ARCH_HH="${DEST}/sleigh_arch.hh"
if [ -f "$SLEIGH_ARCH_HH" ] && ! grep -q "getLanguageDescriptions" "$SLEIGH_ARCH_HH" 2>/dev/null; then
    echo "  Patching sleigh_arch.hh (collectSpecFiles -> public, add getLanguageDescriptions)..."
    # Remove collectSpecFiles from protected section
    sed -i '' '/^  static void collectSpecFiles(ostream &errs);/d' "$SLEIGH_ARCH_HH"
    # Add after getDescription line in public section
    sed -i '' '/virtual string getDescription(void) const;/a\
\
  static void collectSpecFiles(ostream \&errs);\
  static const vector<LanguageDescription> \&getLanguageDescriptions() { return description; }
' "$SLEIGH_ARCH_HH"
    echo "  Done."
else
    echo "  sleigh_arch.hh: already patched or not found."
fi

# Patch: Add getBaseState to ParserContext (context.hh)
CONTEXT_HH="${DEST}/context.hh"
if [ -f "$CONTEXT_HH" ] && ! grep -q "getBaseState" "$CONTEXT_HH" 2>/dev/null; then
    echo "  Patching context.hh (add getBaseState)..."
    sed -i '' '/int4 delayslot;/a\
protected:\
  ConstructState **getBaseState(void) { return \&base_state; }
' "$CONTEXT_HH"
    echo "  Done."
else
    echo "  context.hh: already patched or not found."
fi

# Patch: Add getContextCache to Sleigh (sleigh.hh)
SLEIGH_HH="${DEST}/sleigh.hh"
if [ -f "$SLEIGH_HH" ] && ! grep -q "getContextCache" "$SLEIGH_HH" 2>/dev/null; then
    echo "  Patching sleigh.hh (add getContextCache)..."
    sed -i '' '/void resolveHandles(ParserContext &pos) const;/a\
  ContextCache *getContextCache(void) { return cache; }
' "$SLEIGH_HH"
    echo "  Done."
else
    echo "  sleigh.hh: already patched or not found."
fi

# ==============================
# Step 4: Build
# ==============================
echo ""
echo "=== Step 4: Build ==="

mkdir -p "${SCRIPT_DIR}/build"
cd "${SCRIPT_DIR}/build"

echo "Running cmake configure..."
cmake .. -DCMAKE_BUILD_TYPE=Release 2>&1
echo ""

echo "Running cmake build..."
cmake --build . -j$(sysctl -n hw.ncpu 2>/dev/null || echo 4) 2>&1

# ==============================
# Step 5: Test plugin loading
# ==============================
echo ""
echo "=== Step 5: Test plugin loading ==="

DYLIB="${SCRIPT_DIR}/build/lldbghidra.dylib"
if [ -f "$DYLIB" ]; then
    echo "Testing: lldb -o 'plugin load $DYLIB' -o quit"
    lldb -o "plugin load $DYLIB" -o "quit" 2>&1
else
    echo "Build output not found at $DYLIB. Check build errors above."
fi

echo ""
echo "=== Done ==="
