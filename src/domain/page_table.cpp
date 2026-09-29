#include "domain/page_table.h"

namespace vmsim::domain {

PTE& PageTable::entryFor(VirtualAddress va) {
    uint32_t pt1 = va.pt1Index();
    if (!level1_[pt1]) {
        level1_[pt1] = std::make_unique<Level2>();
    }
    return (*level1_[pt1])[va.pt2Index()];
}

bool PageTable::hasLevel2(uint32_t pt1Index) const {
    return static_cast<bool>(level1_[pt1Index]);
}

} // namespace vmsim::domain
