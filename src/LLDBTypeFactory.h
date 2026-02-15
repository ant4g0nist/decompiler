/* lldbghidra - MIT License - Copyright 2026 */

#ifndef LLDBGHIDRA_LLDBTYPEFACTORY_H
#define LLDBGHIDRA_LLDBTYPEFACTORY_H

#include <type.hh>
#include <lldb/API/SBTarget.h>

using namespace ghidra;

namespace lldbghidra {

class LLDBArchitecture;

class LLDBTypeFactory : public TypeFactory {
private:
    LLDBArchitecture *arch;
    lldb::SBTarget target;

protected:
    Datatype *findById(const string &n, uint8 id, int4 sz) override;

public:
    LLDBTypeFactory(LLDBArchitecture *arch, lldb::SBTarget target);
    ~LLDBTypeFactory() override;
};

} // namespace lldbghidra

#endif // LLDBGHIDRA_LLDBTYPEFACTORY_H
