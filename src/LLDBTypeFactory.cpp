/* lldbghidra - MIT License - Copyright 2026 */

#include "LLDBTypeFactory.h"
#include "LLDBArchitecture.h"
#include <lldb/API/SBType.h>

namespace lldbghidra {

LLDBTypeFactory::LLDBTypeFactory(LLDBArchitecture *arch, lldb::SBTarget target)
    : TypeFactory(arch), arch(arch), target(target) {
}

LLDBTypeFactory::~LLDBTypeFactory() {
}

Datatype *LLDBTypeFactory::findById(const string &n, uint8 id, int4 sz) {
    // First check the internal cache
    Datatype *result = TypeFactory::findById(n, id, sz);
    if (result) return result;

    // Phase 1: rely on core types registered in buildCoreTypes.
    // Phase 2: query LLDB's type system via target.FindTypes(n)
    // and convert SBType -> Ghidra Datatype.
    return nullptr;
}

} // namespace lldbghidra
