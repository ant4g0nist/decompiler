/* lldbghidra - MIT License - Copyright 2026 */

#ifndef LLDBGHIDRA_LLDBSCOPE_H
#define LLDBGHIDRA_LLDBSCOPE_H

#include <database.hh>
#include <lldb/API/SBTarget.h>
#include <lldb/API/SBProcess.h>
#include <lldb/API/SBModule.h>
#include <lldb/API/SBSymbol.h>
#include <lldb/API/SBFunction.h>
#include <memory>

// Windows defines LoadImage to LoadImageA
#ifdef LoadImage
#undef LoadImage
#endif

using namespace ghidra;

namespace lldbghidra {

class LLDBArchitecture;

class LLDBScope : public Scope {
private:
    LLDBArchitecture *arch;
    ScopeInternal *cache;
    std::unique_ptr<uint8> next_id;

    uint8 makeId() const { return (*next_id)++; }

    FunctionSymbol *registerFunction(lldb::SBSymbol &symbol) const;
    Symbol *queryLLDBAbsolute(uint64_t addr, bool contain) const;
    Symbol *queryLLDB(const Address &addr, bool contain) const;

protected:
    void removeRange(AddrSpace *spc, uintb first, uintb last) override {
        throw LowlevelError("removeRange not supported");
    }
    void addSymbolInternal(Symbol *sym) override {
        throw LowlevelError("addSymbolInternal unimplemented");
    }
    SymbolEntry *addMapInternal(Symbol *sym, uint4 exfl, const Address &addr,
                                int4 off, int4 sz, const RangeList &uselim) override {
        throw LowlevelError("addMapInternal unimplemented");
    }
    SymbolEntry *addDynamicMapInternal(Symbol *sym, uint4 exfl, uint8 hash,
                                       int4 off, int4 sz, const RangeList &uselim) override {
        throw LowlevelError("addDynamicMapInternal unimplemented");
    }

public:
    explicit LLDBScope(LLDBArchitecture *arch);
    ~LLDBScope() override;

    Scope *buildSubScope(uint8 id, const string &nm) override;
    void clear() override { cache->clear(); }

    SymbolEntry *addSymbol(const string &name, Datatype *ct, const Address &addr,
                           const Address &usepoint) override {
        return cache->addSymbol(name, ct, addr, usepoint);
    }
    string buildVariableName(const Address &addr, const Address &pc, Datatype *ct,
                             int4 &index, uint4 flags) const override {
        return cache->buildVariableName(addr, pc, ct, index, flags);
    }
    string buildUndefinedName() const override { return cache->buildUndefinedName(); }
    void setAttribute(Symbol *sym, uint4 attr) override { cache->setAttribute(sym, attr); }
    void clearAttribute(Symbol *sym, uint4 attr) override { cache->clearAttribute(sym, attr); }
    void setDisplayFormat(Symbol *sym, uint4 attr) override { cache->setDisplayFormat(sym, attr); }
    void adjustCaches() override { cache->adjustCaches(); }

    SymbolEntry *findAddr(const Address &addr, const Address &usepoint) const override;
    SymbolEntry *findContainer(const Address &addr, int4 size, const Address &usepoint) const override;
    SymbolEntry *findClosestFit(const Address &addr, int4 size, const Address &usepoint) const override {
        throw LowlevelError("findClosestFit unimplemented");
    }
    Funcdata *findFunction(const Address &addr) const override;
    ExternRefSymbol *findExternalRef(const Address &addr) const override;
    LabSymbol *findCodeLabel(const Address &addr) const override;
    bool isNameUsed(const string &name, const Scope *op2) const override {
        throw LowlevelError("isNameUsed unimplemented");
    }
    Funcdata *resolveExternalRefFunction(ExternRefSymbol *sym) const override;

    SymbolEntry *findOverlap(const Address &addr, int4 size) const override {
        throw LowlevelError("findOverlap unimplemented");
    }
    SymbolEntry *findBefore(const Address &addr) const {
        throw LowlevelError("findBefore unimplemented");
    }
    SymbolEntry *findAfter(const Address &addr) const {
        throw LowlevelError("findAfter unimplemented");
    }
    void findByName(const string &name, vector<Symbol *> &res) const override {
        throw LowlevelError("findByName unimplemented");
    }
    MapIterator begin() const override { throw LowlevelError("begin unimplemented"); }
    MapIterator end() const override { throw LowlevelError("end unimplemented"); }
    list<SymbolEntry>::const_iterator beginDynamic() const override { throw LowlevelError("beginDynamic unimplemented"); }
    list<SymbolEntry>::const_iterator endDynamic() const override { throw LowlevelError("endDynamic unimplemented"); }
    list<SymbolEntry>::iterator beginDynamic() override { throw LowlevelError("beginDynamic unimplemented"); }
    list<SymbolEntry>::iterator endDynamic() override { throw LowlevelError("endDynamic unimplemented"); }
    void clearCategory(int4 cat) override { throw LowlevelError("clearCategory unimplemented"); }
    void clearUnlockedCategory(int4 cat) override { throw LowlevelError("clearUnlockedCategory unimplemented"); }
    void clearUnlocked() override { throw LowlevelError("clearUnlocked unimplemented"); }
    void restrictScope(Funcdata *f) override { throw LowlevelError("restrictScope unimplemented"); }
    void removeSymbolMappings(Symbol *symbol) override { throw LowlevelError("removeSymbolMappings unimplemented"); }
    void removeSymbol(Symbol *symbol) override { throw LowlevelError("removeSymbol unimplemented"); }
    void renameSymbol(Symbol *sym, const string &newname) override { throw LowlevelError("renameSymbol unimplemented"); }
    void retypeSymbol(Symbol *sym, Datatype *ct) override { throw LowlevelError("retypeSymbol unimplemented"); }
    string makeNameUnique(const string &nm) const override { throw LowlevelError("makeNameUnique unimplemented"); }
    void encode(Encoder &encoder) const override { cache->encode(encoder); }
    void decode(Decoder &decoder) override { throw LowlevelError("decode unimplemented"); }
    void printEntries(ostream &s) const override { throw LowlevelError("printEntries unimplemented"); }
    int4 getCategorySize(int4 cat) const override { throw LowlevelError("getCategorySize unimplemented"); }
    Symbol *getCategorySymbol(int4 cat, int4 ind) const override { throw LowlevelError("getCategorySymbol unimplemented"); }
    void setCategory(Symbol *sym, int4 cat, int4 ind) override { throw LowlevelError("setCategory unimplemented"); }
};

} // namespace lldbghidra

#endif // LLDBGHIDRA_LLDBSCOPE_H
