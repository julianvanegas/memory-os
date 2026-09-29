#include "infrastructure/report_printer.h"
#include <iomanip>
#include <iostream>

namespace vmsim::infrastructure {

void ReportPrinter::print(const domain::Stats& stats, PolicyKind policy) {
    std::cout << "\n--- Estadisticas finales ---\n";
    std::cout << "Total de accesos: " << stats.totalAccesses << "\n";
    std::cout << "Total fallos de pagina: " << stats.pageFaults << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Hit rate: " << stats.hitRate() << "%\n";
    std::cout << "Total reemplazos: " << stats.replacements << "\n";
    std::cout << "Politica: " << (policy == PolicyKind::Fifo ? "FIFO" : "LRU") << "\n";
    std::cout << "Tiempo de procesamiento: " << stats.elapsedMs << " ms\n";
}

} // namespace vmsim::infrastructure
