#pragma once
#include <memory>

#include "domain/replacement_policy.h"
#include "infrastructure/cli_args.h"

namespace vmsim::infrastructure {

std::unique_ptr<domain::IReplacementPolicy>
makeReplacementPolicy(PolicyKind policy);

} // namespace vmsim::infrastructure
