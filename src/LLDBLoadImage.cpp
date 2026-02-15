/* lldbghidra - MIT License - Copyright 2026 */

#include "LLDBLoadImage.h"
#include <lldb/API/SBError.h>
#include <lldb/API/SBModule.h>
#include <lldb/API/SBSection.h>
#include <cstring>

namespace lldbghidra {

LLDBLoadImage::LLDBLoadImage(lldb::SBTarget target, lldb::SBProcess process,
                             AddrSpaceManager *addrSpaceManager)
    : LoadImage("lldb_program"),
      target(target),
      process(process),
      addrSpaceManager(addrSpaceManager) {
}

void LLDBLoadImage::loadFill(uint1 *ptr, int4 size, const Address &addr) {
    if (ptr == nullptr || size <= 0) return;
    const size_t req = static_cast<size_t>(size);

    lldb::SBError error;
    size_t bytesRead = 0;

    if (process.IsValid()) {
        bytesRead = process.ReadMemory(addr.getOffset(), ptr, req, error);
    }

    // If process read failed or no process, try reading from target (static)
    if (bytesRead == 0 || error.Fail()) {
        lldb::SBAddress sbAddr;
        sbAddr.SetLoadAddress(addr.getOffset(), target);
        if (sbAddr.IsValid()) {
            bytesRead = target.ReadMemory(sbAddr, ptr, req, error);
        }
    }

    if (bytesRead > req) bytesRead = req;

    // Zero-fill any bytes we couldn't read
    if (bytesRead < req) {
        memset(ptr + bytesRead, 0, req - bytesRead);
    }
}

string LLDBLoadImage::getArchType() const {
    return "lldb";
}

void LLDBLoadImage::adjustVma(long adjust) {
    throw LowlevelError("Cannot adjust LLDB virtual memory");
}

void LLDBLoadImage::getReadonly(RangeList &list) const {
    auto space = addrSpaceManager->getDefaultCodeSpace();
    uint32_t numModules = target.GetNumModules();

    for (uint32_t i = 0; i < numModules; i++) {
        lldb::SBModule module = target.GetModuleAtIndex(i);
        uint32_t numSections = module.GetNumSections();

        for (uint32_t j = 0; j < numSections; j++) {
            lldb::SBSection section = module.GetSectionAtIndex(j);
            if (!section.IsValid()) continue;

            uint32_t perms = section.GetPermissions();
            // If permissions are known and section is not writable, mark as readonly
            if (perms != 0 && !(perms & lldb::ePermissionsWritable)) {
                lldb::addr_t loadAddr = section.GetLoadAddress(target);
                if (loadAddr != LLDB_INVALID_ADDRESS) {
                    lldb::addr_t sz = section.GetByteSize();
                    if (sz > 0) {
                        list.insertRange(space, loadAddr, loadAddr + sz - 1);
                    }
                }
            }
        }
    }
}

} // namespace lldbghidra
