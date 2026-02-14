/* lldbghidra - LGPL - Copyright 2026 */

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

    } catch (const LowlevelError &e) {
        fprintf(stderr, "[lldbghidra] Error initializing decompiler: %s\n", e.explain.c_str());
    } catch (const std::exception &e) {
        fprintf(stderr, "[lldbghidra] Error initializing decompiler: %s\n", e.what());
    } catch (...) {
        fprintf(stderr, "[lldbghidra] Unknown error initializing decompiler\n");
    }

    lldb::SBCommandInterpreter interp = debugger.GetCommandInterpreter();
    interp.AddCommand("decompile",
                      new lldbghidra::DecompileCommand(),
                      "Decompile a function using Ghidra's decompiler. "
                      "Usage: decompile [address|symbol_name]");

    fprintf(stderr, "[lldbghidra] Plugin loaded. Use 'decompile' command.\n");
    return true;
}
