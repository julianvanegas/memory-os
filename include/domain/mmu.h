#pragma once
#include "domain/page_table.h"
#include "domain/physical_memory.h"
#include "domain/replacement_policy.h"
#include "domain/stats.h"
#include "domain/types.h"

namespace vmsim::domain {

// El corazon del simulador: traduccion VA->PA + manejo de fallos de pagina.
// Une PageTable + PhysicalMemory + la politica de reemplazo inyectada.
// No hay swap simulado: al recargar una pagina previamente evictada, su
// contenido se reinicia en cero (limitacion documentada en el reporte).
class Mmu {
public:
    Mmu(PhysicalMemory& physMem, IReplacementPolicy& policy, Stats& stats);

    uint8_t* translate(VirtualAddress va, bool isWrite);

    // Desmapea una pagina (usado cuando VirtualAllocator::free tiene exito).
    void unmapPage(VirtualAddress va);

private:
    uint32_t handleFault(VirtualAddress va);

    PageTable table_;
    PhysicalMemory& physMem_;
    IReplacementPolicy& policy_;
    Stats& stats_;
};

} // namespace vmsim::domain
