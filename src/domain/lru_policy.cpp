#include "domain/lru_policy.h"
#include <stdexcept>

namespace vmsim::domain {

void LruPolicy::touch(uint32_t frame) {
    auto it = index_.find(frame);
    if (it != index_.end()) {
        order_.erase(it->second);
    }
    order_.push_back(frame);
    index_[frame] = std::prev(order_.end());
}

void LruPolicy::onFrameLoaded(uint32_t frame) {
    touch(frame);
}

void LruPolicy::onAccess(uint32_t frame) {
    touch(frame);
}

uint32_t LruPolicy::pickVictim() {
    if (order_.empty()) {
        throw std::runtime_error("LruPolicy: no hay frames para reemplazar");
    }
    return order_.front();
}

void LruPolicy::onFrameFreed(uint32_t frame) {
    auto it = index_.find(frame);
    if (it != index_.end()) {
        order_.erase(it->second);
        index_.erase(it);
    }
}

} // namespace vmsim::domain
