#pragma once
#include <list>
#include <unordered_map>
#include "domain/replacement_policy.h"

namespace vmsim::domain {

// FIFO: reemplaza el frame cargado hace mas tiempo, sin importar accesos
// posteriores (onAccess es un no-op deliberado).
class FifoPolicy : public IReplacementPolicy {
public:
    void onFrameLoaded(uint32_t frame) override;
    void onAccess(uint32_t frame) override;
    uint32_t pickVictim() override;
    void onFrameFreed(uint32_t frame) override;
    const char* name() const override { return "FIFO"; }

private:
    std::list<uint32_t> order_;
    std::unordered_map<uint32_t, std::list<uint32_t>::iterator> index_;
};

} // namespace vmsim::domain
