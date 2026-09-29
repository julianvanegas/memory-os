#pragma once
#include <list>
#include <unordered_map>
#include "domain/replacement_policy.h"

namespace vmsim::domain {

// LRU: misma estructura de datos que FIFO (lista en orden de "reciente uso" +
// mapa de iteradores), pero onAccess TAMBIEN mueve el frame al extremo mas
// reciente. Esa es la unica diferencia real entre las dos politicas.
class LruPolicy : public IReplacementPolicy {
public:
    void onFrameLoaded(uint32_t frame) override;
    void onAccess(uint32_t frame) override;
    uint32_t pickVictim() override;
    void onFrameFreed(uint32_t frame) override;
    const char* name() const override { return "LRU"; }

private:
    void touch(uint32_t frame);

    std::list<uint32_t> order_;
    std::unordered_map<uint32_t, std::list<uint32_t>::iterator> index_;
};

} // namespace vmsim::domain
