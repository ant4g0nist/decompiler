/* lldbghidra - LGPL - Copyright 2026 */

#include "LLDBPrintC.h"
#include <architecture.hh>
#include <varnode.hh>

namespace lldbghidra {

// Constructing this registers the capability
LLDBPrintCCapability LLDBPrintCCapability::inst;

LLDBPrintCCapability::LLDBPrintCCapability() {
    name = "lldb-c-language";
    isdefault = false;
}

PrintLanguage *LLDBPrintCCapability::buildLanguage(Architecture *glb) {
    return new LLDBPrintC(glb, name);
}

LLDBPrintC::LLDBPrintC(Architecture *g, const string &nm) : PrintC(g, nm) {
    option_NULL = true;
    option_unplaced = false;
}

void LLDBPrintC::setOptionNoCasts(bool nc) {
    option_nocasts = nc;
}

void LLDBPrintC::pushUnnamedLocation(const Address &addr, const Varnode *vn, const PcodeOp *op) {
    AddrSpace *space = addr.getSpace();
    if (space->getType() == IPTR_PROCESSOR) {
        pushOp(&dereference, op);
        auto type = glb->types->getTypePointer(space->getAddrSize(), vn->getType(), space->getWordSize());
        pushConstant(addr.getOffset(), type, vartoken, vn, op);
    } else {
        PrintC::pushUnnamedLocation(addr, vn, op);
    }
}

} // namespace lldbghidra
