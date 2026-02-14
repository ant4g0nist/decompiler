/* lldbghidra - LGPL - Copyright 2026 */

#ifndef LLDBGHIDRA_ARCHMAP_H
#define LLDBGHIDRA_ARCHMAP_H

#include <string>

namespace lldbghidra {

/// Map an LLDB target triple (e.g. "arm64-apple-macosx15.0.0") to a SLEIGH language ID
/// (e.g. "AARCH64:LE:64:v8A")
std::string SleighIdFromTriple(const std::string &triple);

/// Map binary format description + language ID to compiler spec ID
std::string CompilerFromFormat(const std::string &format, const std::string &sleighLangId);

} // namespace lldbghidra

#endif // LLDBGHIDRA_ARCHMAP_H
