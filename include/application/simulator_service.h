#pragma once
#include <vector>
#include "application/command.h"
#include "domain/mmu.h"
#include "domain/physical_memory.h"
#include "domain/replacement_policy.h"
#include "domain/stats.h"
#include "domain/virtual_allocator.h"

namespace vmsim::application {

// El unico caso de uso real del programa: dado un conjunto de comandos ya
// parseados, orquesta el dominio (allocator + mmu) y devuelve las
// estadisticas finales. Sin I/O: no abre archivos, no lee argv, no imprime
// el reporte final (eso es responsabilidad de infrastructure).
class SimulatorService {
public:
    SimulatorService(domain::PhysicalMemory& physMem, domain::IReplacementPolicy& policy);

    domain::Stats run(const std::vector<Command>& commands);

private:
    void handleAlloc(const Command& cmd);
    void handleWrite(const Command& cmd);
    void handleRead(const Command& cmd);
    void handleFree(const Command& cmd);

    domain::VirtualAllocator allocator_;
    domain::Stats stats_;
    domain::Mmu mmu_;
};

} // namespace vmsim::application
