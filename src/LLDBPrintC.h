/* lldbghidra - LGPL - Copyright 2026 */

#ifndef LLDBGHIDRA_LLDBPRINTC_H
#define LLDBGHIDRA_LLDBPRINTC_H

#include <printc.hh>

using namespace ghidra;

namespace lldbghidra {

class LLDBPrintC : public PrintC {
protected:
    void pushUnnamedLocation(const Address &addr, const Varnode *vn, const PcodeOp *op) override;

public:
    explicit LLDBPrintC(Architecture *g, const string &nm = "c-language");
    void setOptionNoCasts(bool nc);
    void enableAnsiColors(void);
};

class LLDBPrintCCapability : public PrintLanguageCapability {
private:
    static LLDBPrintCCapability inst;
    LLDBPrintCCapability();

public:
    PrintLanguage *buildLanguage(Architecture *glb) override;
};

} // namespace lldbghidra

#endif // LLDBGHIDRA_LLDBPRINTC_H
