/* lldbghidra - LGPL - Copyright 2026 */

#include "LLDBScope.h"
#include "LLDBArchitecture.h"

#include <funcdata.hh>
#include <lldb/API/SBAddress.h>
#include <lldb/API/SBSymbolContext.h>
#include <lldb/API/SBSymbolContextList.h>

namespace lldbghidra {

LLDBScope::LLDBScope(LLDBArchitecture *arch)
    : Scope(0, "", arch, this),
      arch(arch),
      cache(new ScopeInternal(0, "lldb-internal", arch, this)),
      next_id(new uint8) {
    *next_id = 1;
}

LLDBScope::~LLDBScope() {
    delete cache;
}

Scope *LLDBScope::buildSubScope(uint8 id, const string &nm) {
    return new ScopeInternal(id, nm, arch);
}

FunctionSymbol *LLDBScope::registerFunction(lldb::SBSymbol &symbol) const {
    if (!symbol.IsValid()) return nullptr;

    lldb::SBTarget target = arch->getTarget();
    lldb::addr_t startAddr = symbol.GetStartAddress().GetLoadAddress(target);
    lldb::addr_t endAddr = symbol.GetEndAddress().GetLoadAddress(target);

    if (startAddr == LLDB_INVALID_ADDRESS) return nullptr;
    if (endAddr == LLDB_INVALID_ADDRESS || endAddr <= startAddr) {
        endAddr = startAddr + 1; // minimal function
    }

    Address funcAddr(arch->getDefaultCodeSpace(), startAddr);

    // Check cache first
    Funcdata *existing = cache->findFunction(funcAddr);
    if (existing) return existing->getSymbol();

    const char *name = symbol.GetName();
    if (!name || !name[0]) name = "func_unknown";

    // Demangle if possible
    const char *displayName = symbol.GetDisplayName();
    std::string funcName = displayName ? displayName : name;

    // Use addFunction on cache to register it
    // The Scope interface expects us to create a function through XML or
    // through addFunction. We use the internal cache for simplicity.
    FunctionSymbol *funcSym = cache->addFunction(funcAddr, funcName);
    return funcSym;
}

Symbol *LLDBScope::queryLLDBAbsolute(uint64_t addr, bool contain) const {
    lldb::SBTarget target = arch->getTarget();
    lldb::SBAddress sbAddr;
    sbAddr.SetLoadAddress(addr, target);
    if (!sbAddr.IsValid()) return nullptr;

    lldb::SBSymbolContext ctx = target.ResolveSymbolContextForAddress(
        sbAddr,
        lldb::eSymbolContextSymbol | lldb::eSymbolContextFunction
    );

    lldb::SBSymbol symbol = ctx.GetSymbol();
    if (!symbol.IsValid()) return nullptr;

    lldb::addr_t symStart = symbol.GetStartAddress().GetLoadAddress(target);

    if (!contain && symStart != addr) return nullptr;

    if (symbol.GetType() == lldb::eSymbolTypeCode ||
        symbol.GetType() == lldb::eSymbolTypeResolver) {
        lldb::SBSymbol mutableSym = symbol;
        FunctionSymbol *funcSym = registerFunction(mutableSym);
        if (funcSym) return funcSym;
    }

    return nullptr;
}

Symbol *LLDBScope::queryLLDB(const Address &addr, bool contain) const {
    return queryLLDBAbsolute(addr.getOffset(), contain);
}

SymbolEntry *LLDBScope::findAddr(const Address &addr, const Address &usepoint) const {
    SymbolEntry *entry = cache->findAddr(addr, usepoint);
    if (entry) return entry;

    Symbol *sym = queryLLDB(addr, false);
    if (sym) {
        return sym->getMapEntry(addr);
    }
    return nullptr;
}

SymbolEntry *LLDBScope::findContainer(const Address &addr, int4 size, const Address &usepoint) const {
    SymbolEntry *entry = cache->findContainer(addr, size, usepoint);
    if (entry) return entry;

    Symbol *sym = queryLLDB(addr, true);
    if (sym) {
        return sym->getMapEntry(addr);
    }
    return nullptr;
}

Funcdata *LLDBScope::findFunction(const Address &addr) const {
    Funcdata *fd = cache->findFunction(addr);
    if (fd) return fd;

    // Try to find via LLDB
    Symbol *sym = queryLLDB(addr, false);
    if (sym) {
        fd = cache->findFunction(addr);
    }
    return fd;
}

ExternRefSymbol *LLDBScope::findExternalRef(const Address &addr) const {
    return cache->findExternalRef(addr);
}

LabSymbol *LLDBScope::findCodeLabel(const Address &addr) const {
    return cache->findCodeLabel(addr);
}

Funcdata *LLDBScope::resolveExternalRefFunction(ExternRefSymbol *sym) const {
    return cache->resolveExternalRefFunction(sym);
}

} // namespace lldbghidra
