/* lldbghidra - MIT License - Copyright 2026 */

#include "DecompileCommand.h"
#include "LLDBArchitecture.h"

#include <libdecomp.hh>
#include <sleigh_arch.hh>
#include <lldb/API/SBDebugger.h>
#include <lldb/API/SBCommandInterpreter.h>
#include <cstdlib>
#include <sstream>

using namespace ghidra;

namespace lldb {
    bool PluginInitialize(lldb::SBDebugger debugger);
}

bool lldb::PluginInitialize(lldb::SBDebugger debugger) {
    bool init_ok = true;
    try {
        const char *sleighHome = std::getenv("SLEIGHHOME");
        if (!sleighHome || sleighHome[0] == '\0') {
            fprintf(stderr, "[lldbghidra] WARNING: SLEIGHHOME not set. "
                    "Decompilation will fail without SLEIGH spec files.\n");
            startDecompilerLibrary((const char *)nullptr);
        } else {
            startDecompilerLibrary(sleighHome);
        }

        // Ensure language descriptions are loaded
        std::ostringstream specErrs;
        SleighArchitecture::collectSpecFiles(specErrs);
        if (!specErrs.str().empty()) {
            fprintf(stderr, "[lldbghidra] Spec load diagnostics:\n%s\n", specErrs.str().c_str());
        }

    } catch (const LowlevelError &e) {
        fprintf(stderr, "[lldbghidra] Error initializing decompiler: %s\n", e.explain.c_str());
        init_ok = false;
    } catch (const std::exception &e) {
        fprintf(stderr, "[lldbghidra] Error initializing decompiler: %s\n", e.what());
        init_ok = false;
    } catch (...) {
        fprintf(stderr, "[lldbghidra] Unknown error initializing decompiler\n");
        init_ok = false;
    }

    if (!init_ok) {
        fprintf(stderr, "[lldbghidra] Plugin initialization failed; command not registered.\n");
        return false;
    }

    lldb::SBCommandInterpreter interp = debugger.GetCommandInterpreter();
    interp.AddCommand("decompile",
                      new lldbghidra::DecompileCommand(),
                      "Decompile a function using Ghidra's decompiler. "
                      "Usage: decompile [address|symbol_name]");

    fprintf(stderr, "[lldbghidra] Plugin loaded. Use 'decompile' command.\n");
    return true;
}
