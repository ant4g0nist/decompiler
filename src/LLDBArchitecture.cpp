/* lldbghidra - MIT License - Copyright 2026 */

#include "LLDBArchitecture.h"
#include "LLDBLoadImage.h"
#include "LLDBScope.h"
#include "LLDBTypeFactory.h"
#include "LLDBCommentDatabase.h"
#include "ArchMap.h"

#include <funcdata.hh>
#include <coreaction.hh>
#include <iostream>
#include <cstdlib>

namespace lldbghidra {

static std::string getFilenameFromTarget(lldb::SBTarget target) {
    lldb::SBFileSpec fileSpec = target.GetExecutable();
    if (fileSpec.IsValid()) {
        const char *filename = fileSpec.GetFilename();
        if (filename) return filename;
    }
    return "unknown";
}

LLDBArchitecture::LLDBArchitecture(lldb::SBTarget target, lldb::SBProcess process,
                                   const std::string &sleigh_id)
    : SleighArchitecture(getFilenameFromTarget(target), sleigh_id, &std::cout),
      _target(target),
      _process(process) {
    collectSpecFiles(std::cerr);
}

void LLDBArchitecture::initSleighHome(const std::string &path) {
    if (!path.empty()) {
        setenv("SLEIGHHOME", path.c_str(), 0);
    }
}

Translate *LLDBArchitecture::buildTranslator(DocumentStorage &store) {
    return SleighArchitecture::buildTranslator(store);
}

void LLDBArchitecture::buildLoader(DocumentStorage &store) {
    loader = new LLDBLoadImage(_target, _process, this);
}

Scope *LLDBArchitecture::buildDatabase(DocumentStorage &store) {
    symboltab = new Database(this, false);
    Scope *globalscope = new LLDBScope(this);
    symboltab->attachScope(globalscope, nullptr);
    return globalscope;
}

void LLDBArchitecture::buildTypegrp(DocumentStorage &store) {
    lldbTypeFactory = new LLDBTypeFactory(this, _target);
    types = lldbTypeFactory;
}

void LLDBArchitecture::buildCoreTypes(DocumentStorage &store) {
    types->setCoreType("void", 1, TYPE_VOID, false);

    types->setCoreType("bool", 1, TYPE_BOOL, false);
    types->setCoreType("bool4", 4, TYPE_BOOL, false);
    types->setCoreType("bool8", 8, TYPE_BOOL, false);

    types->setCoreType("uint8_t", 1, TYPE_UINT, false);
    types->setCoreType("uint16_t", 2, TYPE_UINT, false);
    types->setCoreType("uint32_t", 4, TYPE_UINT, false);
    types->setCoreType("uint64_t", 8, TYPE_UINT, false);
    types->setCoreType("int8_t", 1, TYPE_INT, false);
    types->setCoreType("int16_t", 2, TYPE_INT, false);
    types->setCoreType("int32_t", 4, TYPE_INT, false);
    types->setCoreType("int64_t", 8, TYPE_INT, false);
    types->setCoreType("int", sizeof(int), TYPE_INT, false);

    types->setCoreType("double", 8, TYPE_FLOAT, false);
    types->setCoreType("float", 4, TYPE_FLOAT, false);
    types->setCoreType("float8", 8, TYPE_FLOAT, false);
    types->setCoreType("float10", 10, TYPE_FLOAT, false);
    types->setCoreType("float16", 16, TYPE_FLOAT, false);

    types->setCoreType("uchar", 1, TYPE_UNKNOWN, false);
    types->setCoreType("ushort", 2, TYPE_UNKNOWN, false);
    types->setCoreType("uint", 4, TYPE_UNKNOWN, false);
    types->setCoreType("ulong", 8, TYPE_UNKNOWN, false);

    types->setCoreType("code", 1, TYPE_CODE, false);

    types->setCoreType("char", 1, TYPE_INT, true);
    types->setCoreType("wchar", 2, TYPE_INT, true);
    types->setCoreType("char16_t", 2, TYPE_INT, true);
    types->setCoreType("char32_t", 4, TYPE_INT, true);

    types->cacheCoreTypes();
}

void LLDBArchitecture::buildCommentDB(DocumentStorage &store) {
    commentdb = new LLDBCommentDatabase();
}

void LLDBArchitecture::postSpecFile() {
    // No-op for now. Could mark noreturn functions from LLDB's analysis.
}

void LLDBArchitecture::buildAction(DocumentStorage &store) {
    parseExtraRules(store);
    allacts.universalAction(this);
    allacts.resetDefaults();
}

} // namespace lldbghidra
