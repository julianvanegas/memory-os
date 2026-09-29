#pragma once
#include <array>
#include <memory>
#include "domain/types.h"

namespace vmsim::domain {

struct PTE {
    bool valid = false;
    uint32_t frame = 0;
    bool accessed = false;
    bool dirty = false;
};

// Tabla de paginas de dos niveles. El nivel 2 se crea perezosamente la
// primera vez que se pide una entrada bajo un indice de nivel 1 dado.
class PageTable {
public:
    PTE& entryFor(VirtualAddress va);
    bool hasLevel2(uint32_t pt1Index) const;

private:
    using Level2 = std::array<PTE, PT_ENTRIES>;
    std::array<std::unique_ptr<Level2>, PT_ENTRIES> level1_;
};

} // namespace vmsim::domain
