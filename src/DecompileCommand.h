/* lldbghidra - LGPL - Copyright 2026 */

#ifndef LLDBGHIDRA_DECOMPILECOMMAND_H
#define LLDBGHIDRA_DECOMPILECOMMAND_H

#include <lldb/API/SBCommandInterpreter.h>
#include <lldb/API/SBCommandReturnObject.h>
#include <lldb/API/SBDebugger.h>

namespace lldbghidra {

class DecompileCommand : public lldb::SBCommandPluginInterface {
public:
    bool DoExecute(lldb::SBDebugger debugger, char **command,
                   lldb::SBCommandReturnObject &result) override;
};

} // namespace lldbghidra

#endif // LLDBGHIDRA_DECOMPILECOMMAND_H
