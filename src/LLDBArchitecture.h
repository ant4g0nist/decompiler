/* lldbghidra - MIT License - Copyright 2026 */

#ifndef LLDBGHIDRA_LLDBARCHITECTURE_H
#define LLDBGHIDRA_LLDBARCHITECTURE_H

#include "sleigh_arch.hh"
#include <lldb/API/SBTarget.h>
#include <lldb/API/SBProcess.h>

using namespace ghidra;

namespace lldbghidra {

class LLDBTypeFactory;

class LLDBArchitecture : public SleighArchitecture {
private:
    lldb::SBTarget _target;
    lldb::SBProcess _process;
    LLDBTypeFactory *lldbTypeFactory = nullptr;
    std::vector<std::string> warnings;

public:
    LLDBArchitecture(lldb::SBTarget target, lldb::SBProcess process,
                     const std::string &sleigh_id);

    lldb::SBTarget getTarget() const { return _target; }
    lldb::SBProcess getProcess() const { return _process; }
    LLDBTypeFactory *getTypeFactory() const { return lldbTypeFactory; }

    void addWarning(const std::string &warning) { warnings.push_back(warning); }
    const std::vector<std::string> &getWarnings() const { return warnings; }

    static void initSleighHome(const std::string &path);

protected:
    Translate *buildTranslator(DocumentStorage &store) override;
    void buildLoader(DocumentStorage &store) override;
    Scope *buildDatabase(DocumentStorage &store) override;
    void buildTypegrp(DocumentStorage &store) override;
    void buildCoreTypes(DocumentStorage &store) override;
    void buildCommentDB(DocumentStorage &store) override;
    void postSpecFile() override;
    void buildAction(DocumentStorage &store) override;
};

} // namespace lldbghidra

#endif // LLDBGHIDRA_LLDBARCHITECTURE_H
