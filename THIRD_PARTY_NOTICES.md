# Third-Party Notices

This repository vendors third-party source code and headers. If you redistribute
source or binaries built from this repository, you are responsible for complying
with the licenses and preserving required notices.

Project repository: https://github.com/ant4g0nist/decompiler

## Ghidra Decompiler (Vendored)

Upstream: https://github.com/NationalSecurityAgency/ghidra

Path: `third_party/ghidra-decompiler/`

License:
- Apache License 2.0 (Ghidra LICENSE): `third_party/LICENSES/GHIDRA-LICENSE.txt`

Notice:
- `third_party/NOTICE-ghidra.txt` (copied from upstream)

Local modifications:
- This repo includes patches to compile the decompiler standalone (see `README.md`).
- `third_party/ghidra-decompiler/xml.hh` was modified to fix UB (returning a reference to a temporary).

## LLDB API Headers (Vendored)

Upstream: https://github.com/llvm/llvm-project

Path: `src/include/lldb/`

License:
- Apache License 2.0 with LLVM Exceptions (LLVM LICENSE.txt): `third_party/LICENSES/LLVM-LICENSE.txt`
