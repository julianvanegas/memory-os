#pragma once
#include "domain/stats.h"
#include "infrastructure/cli_args.h"

namespace vmsim::infrastructure {

class ReportPrinter {
public:
    static void print(const domain::Stats& stats, PolicyKind policy);
};

} // namespace vmsim::infrastructure
