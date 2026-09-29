#include "domain/fifo_policy.h"
#include <stdexcept>

namespace vmsim::domain {

void FifoPolicy::onFrameLoaded(uint32_t frame) {
    order_.push_back(frame);
    index_[frame] = std::prev(order_.end());
}

void FifoPolicy::onAccess(uint32_t /*frame*/) {
    // FIFO ignora el acceso reciente, no reordena.
}

uint32_t FifoPolicy::pickVictim() {
    if (order_.empty()) {
        throw std::runtime_error("FifoPolicy: no hay frames para reemplazar");
    }
    return order_.front();
}

void FifoPolicy::onFrameFreed(uint32_t frame) {
    auto it = index_.find(frame);
    if (it != index_.end()) {
        order_.erase(it->second);
        index_.erase(it);
    }
}

} // namespace vmsim::domain
