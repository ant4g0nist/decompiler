/* lldbghidra - LGPL - Copyright 2026 */

#ifndef LLDBGHIDRA_LLDBLOADIMAGE_H
#define LLDBGHIDRA_LLDBLOADIMAGE_H

#include "loadimage.hh"
#include "translate.hh"
#include <lldb/API/SBProcess.h>
#include <lldb/API/SBTarget.h>

// Windows defines LoadImage to LoadImageA
#ifdef LoadImage
#undef LoadImage
#endif

using namespace ghidra;

namespace lldbghidra {

class LLDBLoadImage : public LoadImage {
private:
    mutable lldb::SBTarget target;
    mutable lldb::SBProcess process;
    AddrSpaceManager *addrSpaceManager;

public:
    LLDBLoadImage(lldb::SBTarget target, lldb::SBProcess process,
                  AddrSpaceManager *addrSpaceManager);

    void loadFill(uint1 *ptr, int4 size, const Address &addr) override;
    string getArchType() const override;
    void adjustVma(long adjust) override;
    void getReadonly(RangeList &list) const override;
};

} // namespace lldbghidra

#endif // LLDBGHIDRA_LLDBLOADIMAGE_H
