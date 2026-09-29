#include "infrastructure/replacement_policy_factory.h"

#include "domain/fifo_policy.h"
#include "domain/lru_policy.h"

namespace vmsim::infrastructure {

std::unique_ptr<domain::IReplacementPolicy>
makeReplacementPolicy(PolicyKind policy) {
    if (policy == PolicyKind::Fifo) {
        return std::make_unique<domain::FifoPolicy>();
    }
    return std::make_unique<domain::LruPolicy>();
}

} // namespace vmsim::infrastructure
