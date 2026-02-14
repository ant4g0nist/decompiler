/* lldbghidra - LGPL - Copyright 2026 */

#include "DecompileCommand.h"
#include "LLDBArchitecture.h"
#include "LLDBPrintC.h"
#include "ArchMap.h"

#include <libdecomp.hh>
#include <funcdata.hh>
#include <coreaction.hh>

#include <lldb/API/SBTarget.h>
#include <lldb/API/SBProcess.h>
#include <lldb/API/SBThread.h>
#include <lldb/API/SBFrame.h>
#include <lldb/API/SBSymbol.h>
#include <lldb/API/SBFunction.h>
#include <lldb/API/SBAddress.h>
#include <lldb/API/SBModule.h>
#include <lldb/API/SBSymbolContext.h>
#include <lldb/API/SBSymbolContextList.h>
#include <lldb/API/SBStream.h>

#include <sstream>
#include <mutex>
#include <cstdlib>
#include <cstring>

using namespace ghidra;

namespace lldbghidra {

static std::recursive_mutex decompiler_mutex;

static lldb::addr_t parseAddress(const char *str) {
    if (!str) return LLDB_INVALID_ADDRESS;
    char *end;
    lldb::addr_t addr = strtoull(str, &end, 0);
    if (end == str) return LLDB_INVALID_ADDRESS;
    return addr;
}

bool DecompileCommand::DoExecute(lldb::SBDebugger debugger, char **command,
                                  lldb::SBCommandReturnObject &result) {
    std::lock_guard<std::recursive_mutex> lock(decompiler_mutex);

    lldb::SBTarget target = debugger.GetSelectedTarget();
    if (!target.IsValid()) {
        result.SetError("No target selected\n");
        return false;
    }

    lldb::SBProcess process = target.GetProcess();
    if (!process.IsValid()) {
        result.SetError("No process running. Attach to or launch a process first.\n");
        return false;
    }

    // Join all arguments (handles ObjC names like "-[NSObject init]")
    std::string argStr;
    if (command) {
        for (int i = 0; command[i]; i++) {
            if (i > 0) argStr += " ";
            argStr += command[i];
        }
    }

    // Determine the address to decompile
    lldb::addr_t decompAddr = LLDB_INVALID_ADDRESS;

    if (!argStr.empty()) {
        // Try as address first
        decompAddr = parseAddress(argStr.c_str());

        // If it parsed as an address, verify it resolves as a load address.
        // If not, try treating it as a file address and resolve via modules.
        if (decompAddr != LLDB_INVALID_ADDRESS) {
            lldb::SBAddress sbCheck;
            sbCheck.SetLoadAddress(decompAddr, target);
            lldb::SBSymbolContext checkCtx = target.ResolveSymbolContextForAddress(
                sbCheck, lldb::eSymbolContextSymbol);
            if (!checkCtx.GetSymbol().IsValid()) {
                // Try as file address - search all modules
                for (uint32_t i = 0; i < target.GetNumModules(); i++) {
                    lldb::SBModule mod = target.GetModuleAtIndex(i);
                    lldb::SBAddress fileAddr = mod.ResolveFileAddress(decompAddr);
                    if (fileAddr.IsValid()) {
                        lldb::addr_t loadAddr = fileAddr.GetLoadAddress(target);
                        if (loadAddr != LLDB_INVALID_ADDRESS) {
                            decompAddr = loadAddr;
                            break;
                        }
                    }
                }
            }
        }

        if (decompAddr == LLDB_INVALID_ADDRESS) {
            // Try as symbol name
            lldb::SBSymbolContextList contexts = target.FindFunctions(argStr.c_str());
            if (contexts.GetSize() > 0) {
                lldb::SBSymbolContext ctx = contexts.GetContextAtIndex(0);
                lldb::SBFunction func = ctx.GetFunction();
                if (func.IsValid()) {
                    decompAddr = func.GetStartAddress().GetLoadAddress(target);
                } else {
                    lldb::SBSymbol sym = ctx.GetSymbol();
                    if (sym.IsValid()) {
                        decompAddr = sym.GetStartAddress().GetLoadAddress(target);
                    }
                }
            }
        }

        if (decompAddr == LLDB_INVALID_ADDRESS) {
            std::string err = "Could not find function or address: " + argStr + "\n";
            result.SetError(err.c_str());
            return false;
        }
    } else {
        // No argument — use current frame's PC
        lldb::SBThread thread = process.GetSelectedThread();
        if (!thread.IsValid()) {
            result.SetError("No selected thread\n");
            return false;
        }
        lldb::SBFrame frame = thread.GetSelectedFrame();
        if (!frame.IsValid()) {
            result.SetError("No selected frame\n");
            return false;
        }
        decompAddr = frame.GetPC();
    }

    // Resolve the function at this address
    lldb::SBAddress sbAddr;
    sbAddr.SetLoadAddress(decompAddr, target);
    lldb::SBSymbolContext ctx = target.ResolveSymbolContextForAddress(
        sbAddr, lldb::eSymbolContextSymbol | lldb::eSymbolContextFunction);

    lldb::SBSymbol symbol = ctx.GetSymbol();
    lldb::addr_t funcStart = decompAddr;
    std::string funcName = "unknown";

    if (symbol.IsValid()) {
        lldb::addr_t symStart = symbol.GetStartAddress().GetLoadAddress(target);
        if (symStart != LLDB_INVALID_ADDRESS) {
            funcStart = symStart;
        }
        const char *name = symbol.GetDisplayName();
        if (!name) name = symbol.GetName();
        if (name) funcName = name;
    }

    // Get SLEIGH ID from target triple
    std::string triple = target.GetTriple();
    std::string sleighId;
    try {
        sleighId = SleighIdFromTriple(triple);

        // Append compiler spec from binary format
        lldb::SBModule module = target.GetModuleAtIndex(0);
        if (module.IsValid()) {
            lldb::SBStream stream;
            module.GetDescription(stream);
            std::string desc = stream.GetData() ? stream.GetData() : "";
            std::string compiler = CompilerFromFormat(desc, sleighId);
            if (!compiler.empty()) {
                sleighId += ":" + compiler;
            }
        }
    } catch (const std::runtime_error &e) {
        std::string err = "Architecture error: ";
        err += e.what();
        err += "\n";
        result.SetError(err.c_str());
        return false;
    }

    // Perform decompilation
    try {
        LLDBArchitecture arch(target, process, sleighId);
        DocumentStorage store;
        arch.init(store);

        Address funcAddr(arch.getDefaultCodeSpace(), funcStart);
        Funcdata *func = arch.symboltab->getGlobalScope()->findFunction(funcAddr);
        if (!func) {
            std::stringstream ss;
            ss << "No function found at address 0x" << std::hex << funcStart << "\n";
            result.SetError(ss.str().c_str());
            return false;
        }

        // Run decompilation
        auto action = arch.allacts.getCurrent();
        action->reset(*func);
        int res = action->perform(*func);
        if (res < 0) {
            result.SetError("Decompilation interrupted\n");
            return false;
        }

        // Print warnings
        for (const auto &warning : arch.getWarnings()) {
            std::string w = "[lldbghidra] " + warning + "\n";
            result.AppendWarning(w.c_str());
        }

        // Output C code
        std::stringstream outStream;
        arch.print->setOutputStream(&outStream);
        arch.setPrintLanguage("lldb-c-language");
        arch.print->docFunction(func);

        std::string output = outStream.str();
        if (!output.empty()) {
            result.AppendMessage(output.c_str());
        }
        result.SetStatus(lldb::eReturnStatusSuccessFinishResult);

    } catch (const LowlevelError &e) {
        std::string err = "Decompiler error: " + e.explain + "\n";
        result.SetError(err.c_str());
        return false;
    } catch (const std::exception &e) {
        std::string err = "Error: ";
        err += e.what();
        err += "\n";
        result.SetError(err.c_str());
        return false;
    }

    return true;
}

} // namespace lldbghidra
