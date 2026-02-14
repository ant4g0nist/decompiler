# lldbghidra

An LLDB plugin that brings Ghidra's native C++ decompiler to LLDB. Decompile functions directly from a live debugging session using the `decompile` command.

Inspired by [r2ghidra](https://github.com/radareorg/r2ghidra), which integrates Ghidra's decompiler into radare2.

## Requirements

- macOS (arm64 or x86_64)
- Xcode (or Command Line Tools) with LLDB
- CMake 3.16+
- zlib
- Ghidra source tree (for the decompiler C++ source and SLEIGH specs)
- r2ghidra source tree (for patches to the decompiler source)

### Expected directory layout

```
Decom/
  ghidra/          # Full Ghidra repository
  r2ghidra/        # r2ghidra source (provides patches)
  decompiler/      # Existing project with bundled LLDB headers
  lldbghidra/      # This project
```

## Quick Start

The included `setup.sh` script handles everything: copying Ghidra decompiler source, applying patches, building, and testing.

```bash
chmod +x setup.sh
./setup.sh
```

This will:
1. Copy Ghidra's decompiler C++ source into `third_party/ghidra-decompiler/`
2. Apply r2ghidra compatibility patches
3. Apply critical patches (public API access for `collectSpecFiles`, `getLanguageDescriptions`, etc.)
4. Build with CMake
5. Test plugin loading in LLDB

## Manual Build

If you prefer to build step by step:

```bash
# 1. Run setup to copy source and apply patches (skip build)
# Or manually copy ghidra decompiler source:
mkdir -p third_party/ghidra-decompiler
cp ../ghidra/Ghidra/Features/Decompiler/src/decompile/cpp/*.{cc,hh,h} third_party/ghidra-decompiler/

# 2. Configure and build
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(sysctl -n hw.ncpu)
```

The build produces `build/lldbghidra.dylib`.

## Compiling SLEIGH Specs

The decompiler needs compiled SLEIGH specification files (`.sla`) at runtime. The build also produces `sleighc` which compiles these from source:

```bash
# Create the spec directory structure
mkdir -p specfiles/Ghidra/Processors/AARCH64/data/languages
mkdir -p specfiles/Ghidra/Processors/x86/data/languages

# Copy spec source files
GHIDRA=../ghidra/Ghidra/Processors

cp $GHIDRA/AARCH64/data/languages/*.{slaspec,sinc,pspec,cspec,ldefs,opinion} \
   specfiles/Ghidra/Processors/AARCH64/data/languages/

cp $GHIDRA/x86/data/languages/*.{slaspec,sinc,pspec,cspec,ldefs,opinion} \
   specfiles/Ghidra/Processors/x86/data/languages/

# Compile .slaspec -> .sla
SLEIGHC=build/third_party/ghidra-decompiler/sleighc

cd specfiles/Ghidra/Processors/AARCH64/data/languages
$SLEIGHC AARCH64.slaspec

cd ../../x86/data/languages
$SLEIGHC x86.slaspec
$SLEIGHC x86-64.slaspec
```

## Usage

### Loading the plugin

Set `SLEIGHHOME` to point to the compiled spec files, then load the plugin:

```bash
SLEIGHHOME=/path/to/lldbghidra/specfiles lldb
(lldb) plugin load /path/to/lldbghidra/build/lldbghidra.dylib
```

Or add to `~/.lldbinit` for automatic loading:

```
# ~/.lldbinit
command script import -c "import os; os.environ['SLEIGHHOME'] = '/path/to/lldbghidra/specfiles'"
plugin load /path/to/lldbghidra/build/lldbghidra.dylib
```

### Decompiling functions

```
# Decompile by symbol name
(lldb) decompile main
(lldb) decompile factorial

# Decompile by address
(lldb) decompile 0x100003f00

# Decompile current frame (no arguments)
(lldb) decompile
```

A process must be running (attached or launched) for decompilation to work, since the plugin reads memory from the live process.

## Architecture

The plugin bridges LLDB's debugging APIs to Ghidra's decompiler through these components:

| Component | Description |
|---|---|
| `Plugin.cpp` | LLDB plugin entry point. Initializes the decompiler library and registers the `decompile` command. |
| `DecompileCommand` | Parses command arguments, resolves symbols/addresses, orchestrates decompilation. |
| `LLDBArchitecture` | Extends `SleighArchitecture`. Wires all components together and configures the decompiler. |
| `LLDBLoadImage` | Extends `LoadImage`. Reads bytes from the live process via `SBProcess::ReadMemory`. |
| `LLDBScope` | Extends `Scope`. Resolves function symbols by querying LLDB's symbol tables. |
| `LLDBTypeFactory` | Extends `TypeFactory`. Provides type information (stub, delegates to base). |
| `LLDBCommentDatabase` | Extends `CommentDatabase`. Comment storage (stub). |
| `LLDBPrintC` | Extends `PrintC`. Customizes C output (shows dereferenced pointers, NULL literals). |
| `ArchMap` | Maps LLDB target triples to SLEIGH language IDs and compiler specs. |

### Supported architectures

| LLDB Triple | SLEIGH ID |
|---|---|
| `arm64-*`, `aarch64-*`, `arm64e-*` | `AARCH64:LE:64:v8A` |
| `x86_64-*` | `x86:LE:64:default` |
| `i386-*`, `i686-*` | `x86:LE:32:default` |
| `arm-*`, `armv7-*` | `ARM:LE:32:v7` |

## License

LGPL (same as Ghidra's decompiler)
