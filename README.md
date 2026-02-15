# lldbghidra

An LLDB plugin that brings Ghidra's native C++ decompiler to LLDB. Decompile functions directly from a live debugging session using the `decompile` command, with ANSI syntax highlighting.

Inspired by [r2ghidra](https://github.com/radareorg/r2ghidra), which integrates Ghidra's decompiler into radare2.

## Requirements

### macOS
- macOS (arm64 or x86_64)
- Xcode (or Command Line Tools) with LLDB
- CMake 3.16+
- zlib

### Linux
- LLDB and liblldb-dev (e.g., `lldb-18` and `liblldb-18-dev` on Ubuntu)
- CMake 3.16+
- C++20 compiler (g++ 11+ or clang++ 14+)
- zlib (`zlib1g-dev` on Ubuntu)

## Build

### macOS

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(sysctl -n hw.ncpu)
```

This produces `build/lldbghidra.dylib`.

### Linux

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

This produces `build/lldbghidra.so`.

If LLDB is installed in a non-standard location, pass `-DLLDB_DIR=/path/to/llvm` to cmake.

### Docker

```bash
docker build -t lldbghidra .

# Run decompilation tests (requires ptrace permissions)
docker run --rm --security-opt seccomp=unconfined --cap-add=SYS_PTRACE \
    lldbghidra bash /src/test/docker_test.sh
```

## Usage

### Loading the plugin

Set `SLEIGHHOME` to point to the compiled spec files, then load the plugin:

```bash
# macOS
SLEIGHHOME=/path/to/decompiler/specfiles lldb
(lldb) plugin load /path/to/decompiler/build/lldbghidra.dylib

# Linux
SLEIGHHOME=/path/to/decompiler/specfiles lldb-18
(lldb) plugin load /path/to/decompiler/build/lldbghidra.so
```

Or add to `~/.lldbinit` for automatic loading:

```
# ~/.lldbinit
command script import -c "import os; os.environ['SLEIGHHOME'] = '/path/to/decompiler/specfiles'"
# macOS: plugin load /path/to/decompiler/build/lldbghidra.dylib
# Linux: plugin load /path/to/decompiler/build/lldbghidra.so
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

### Syntax highlighting

When running in a terminal, output is automatically syntax-highlighted with ANSI colors:

| Token | Color |
|---|---|
| Keywords (`return`, `if`, `void`) | Bold magenta |
| Types (`int`, `uint`) | Green |
| Function names | Bold blue |
| Constants | Cyan |
| Parameters | Italic cyan |
| Global variables | Bold yellow |
| Comments | Dim gray |

Colors are automatically disabled when output is piped or redirected.

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
| `EmitAnsi` | Extends `Emit`. ANSI terminal syntax highlighting for decompiled output. |
| `ArchMap` | Maps LLDB target triples to SLEIGH language IDs and compiler specs. |

### Supported architectures

| LLDB Triple | SLEIGH ID |
|---|---|
| `arm64-*`, `aarch64-*`, `arm64e-*` | `AARCH64:LE:64:v8A` |
| `x86_64-*` | `x86:LE:64:default` |
| `i386-*`, `i686-*` | `x86:LE:32:default` |
| `arm-*`, `armv7-*` | `ARM:LE:32:v7` |

## Third-party

The `third_party/ghidra-decompiler/` directory contains Ghidra's decompiler C++ source (Apache 2.0), with patches from [r2ghidra](https://github.com/radareorg/r2ghidra) for standalone compilation.

The `src/include/` directory contains LLDB API headers from the [LLVM project](https://github.com/llvm/llvm-project) (Apache 2.0 with LLVM exceptions).

See `THIRD_PARTY_NOTICES.md` for license and notice references.

## License

MIT — see [LICENSE](LICENSE).

Third-party components retain their original licenses (Apache 2.0).
