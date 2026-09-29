#pragma once
#include <cstdint>

namespace vmsim::domain {

struct Stats {
    uint64_t totalAccesses = 0;
    uint64_t pageFaults = 0;
    uint64_t replacements = 0;
    double elapsedMs = 0.0;

    double hitRate() const {
        if (totalAccesses == 0) return 0.0;
        return static_cast<double>(totalAccesses - pageFaults) /
               static_cast<double>(totalAccesses) * 100.0;
    }
};

} // namespace vmsim::domain
