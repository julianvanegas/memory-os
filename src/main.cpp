#include <chrono>
#include <iostream>
#include <memory>

#include "application/simulator_service.h"
#include "domain/physical_memory.h"
#include "infrastructure/cli_args.h"
#include "infrastructure/command_file_parser.h"
#include "infrastructure/report_printer.h"
#include "infrastructure/replacement_policy_factory.h"

using namespace vmsim;

// Composition root: solo wiring. Si esto empieza a acumular logica de
// negocio, esa logica se filtro y pertenece a application/infrastructure.
int main(int argc, char** argv) {
    try {
        infrastructure::Config config = infrastructure::CliArgs::parse(argc, argv);
        auto commands = infrastructure::CommandFileParser::parse(config.inputFile);

        domain::PhysicalMemory physMem(config.physMemBytes);

        auto policy = infrastructure::makeReplacementPolicy(config.policy);

        application::SimulatorService service(physMem, *policy);

        auto start = std::chrono::steady_clock::now();
        domain::Stats stats = service.run(commands);
        auto end = std::chrono::steady_clock::now();
        stats.elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();

        infrastructure::ReportPrinter::print(stats, config.policy);
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error fatal: " << ex.what() << "\n";
        return 1;
    }
}
