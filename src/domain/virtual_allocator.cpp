#include "domain/virtual_allocator.h"

namespace vmsim::domain {

namespace {
uint64_t roundUpToPage(uint32_t bytes) {
    return ((static_cast<uint64_t>(bytes) + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
}
} // namespace

std::optional<uint32_t> VirtualAllocator::alloc(uint32_t bytes) {
    if (bytes == 0) return std::nullopt;

    uint64_t sizeRounded = roundUpToPage(bytes);
    uint64_t base = nextFreeVA_;
    uint64_t end = base + sizeRounded;
    if (end > (1ULL << 32)) {
        return std::nullopt; // excede el espacio virtual de 32 bits
    }

    allocations_.push_back(Allocation{static_cast<uint32_t>(base), sizeRounded, false});
    nextFreeVA_ = end;
    return static_cast<uint32_t>(base);
}

std::optional<Allocation> VirtualAllocator::free(uint32_t baseAddress) {
    for (auto& allocation : allocations_) {
        if (allocation.base == baseAddress && !allocation.freed) {
            allocation.freed = true;
            return allocation;
        }
    }
    return std::nullopt;
}

bool VirtualAllocator::isAllocated(uint32_t address) const {
    for (const auto& allocation : allocations_) {
        if (allocation.freed) continue;
        uint64_t end = static_cast<uint64_t>(allocation.base) + allocation.size;
        if (static_cast<uint64_t>(address) >= allocation.base && address < end) {
            return true;
        }
    }
    return false;
}

} // namespace vmsim::domain
