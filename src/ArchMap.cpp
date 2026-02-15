/* lldbghidra - MIT License - Copyright 2026 */

#include "ArchMap.h"
#include <stdexcept>

namespace lldbghidra {

std::string SleighIdFromTriple(const std::string &triple) {
    auto dash = triple.find('-');
    std::string arch = (dash != std::string::npos) ? triple.substr(0, dash) : triple;

    if (arch == "arm64" || arch == "aarch64" || arch == "arm64e") {
        return "AARCH64:LE:64:v8A";
    }
    if (arch == "x86_64" || arch == "x86_64h") {
        return "x86:LE:64:default";
    }
    if (arch == "x86" || arch == "i386" || arch == "i686") {
        return "x86:LE:32:default";
    }
    if (arch == "arm" || arch == "armv7" || arch == "armv7s" || arch == "armv7k") {
        return "ARM:LE:32:v7";
    }
    if (arch == "thumbv7" || arch == "thumbv7s") {
        return "ARM:LE:32:v7";
    }

    throw std::runtime_error("Unsupported architecture: " + arch +
        ". Supported: arm64, x86_64, x86, arm");
}

std::string CompilerFromFormat(const std::string &format, const std::string &sleighLangId) {
    bool isMachO = (format.find("Mach-O") != std::string::npos ||
                    format.find("mach-o") != std::string::npos);
    bool isPE = (format.find("PE") != std::string::npos);
    bool isAarch64 = (sleighLangId.find("AARCH64") == 0);

    if (isPE) return "windows";
    if (isAarch64) return "default";  // AARCH64.ldefs: default → AARCH64.cspec
    if (isMachO) return "gcc";        // x86.ldefs: gcc → x86-64-gcc.cspec (SysV ABI)
    return "gcc";
}

} // namespace lldbghidra
