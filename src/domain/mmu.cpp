#include "domain/mmu.h"

namespace vmsim::domain {

Mmu::Mmu(PhysicalMemory& physMem, IReplacementPolicy& policy, Stats& stats)
    : physMem_(physMem), policy_(policy), stats_(stats) {}

uint32_t Mmu::handleFault(VirtualAddress va) {
    stats_.pageFaults++;

    uint32_t frame;
    if (physMem_.hasFreeFrame()) {
        frame = physMem_.allocateFrame(va.vpn());
    } else {
        stats_.replacements++;
        uint32_t victimFrame = policy_.pickVictim();
        std::optional<VPN> victimVpn = physMem_.ownerOf(victimFrame);
        if (victimVpn) {
            PTE& victimPte = table_.entryFor(VirtualAddress::fromVpn(*victimVpn));
            victimPte.valid = false;
            victimPte.frame = 0;
        }
        physMem_.freeFrame(victimFrame);
        policy_.onFrameFreed(victimFrame);
        frame = physMem_.allocateFrame(va.vpn());
    }

    physMem_.zeroFrame(frame);
    policy_.onFrameLoaded(frame);
    return frame;
}

uint8_t* Mmu::translate(VirtualAddress va, bool isWrite) {
    stats_.totalAccesses++;
    PTE& pte = table_.entryFor(va);

    if (!pte.valid) {
        uint32_t frame = handleFault(va);
        pte.valid = true;
        pte.frame = frame;
    } else {
        policy_.onAccess(pte.frame);
    }

    pte.accessed = true;
    if (isWrite) pte.dirty = true;

    return physMem_.pointerTo(pte.frame, va.offset());
}

void Mmu::unmapPage(VirtualAddress va) {
    PTE& pte = table_.entryFor(va);
    if (pte.valid) {
        physMem_.freeFrame(pte.frame);
        policy_.onFrameFreed(pte.frame);
        pte.valid = false;
        pte.frame = 0;
    }
}

} // namespace vmsim::domain
