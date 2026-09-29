#include "test_framework.h"
#include "application/simulator_service.h"
#include "domain/fifo_policy.h"

using namespace vmsim::application;
using namespace vmsim::domain;

TEST(simulator_service_processes_alloc_write_read) {
    PhysicalMemory mem(4 * PAGE_SIZE);
    FifoPolicy policy;
    SimulatorService service(mem, policy);

    std::vector<Command> commands = {
        Command{CommandType::Alloc, 0, 8192, 0, 1},
        Command{CommandType::Write, 0, 0, 42, 2},
        Command{CommandType::Write, 4096, 0, 99, 3},
        Command{CommandType::Read, 0, 0, 0, 4},
        Command{CommandType::Read, 4096, 0, 0, 5},
    };

    Stats stats = service.run(commands);
    ASSERT_EQ(stats.totalAccesses, 4u); // 2 writes + 2 reads (alloc no cuenta)
    ASSERT_EQ(stats.pageFaults, 2u);    // una por cada pagina nueva
}

TEST(simulator_service_read_after_free_is_rejected) {
    PhysicalMemory mem(4 * PAGE_SIZE);
    FifoPolicy policy;
    SimulatorService service(mem, policy);

    std::vector<Command> commands = {
        Command{CommandType::Alloc, 0, 4096, 0, 1},
        Command{CommandType::Free, 0, 0, 0, 2},
        Command{CommandType::Read, 0, 0, 0, 3}, // debe rechazarse (no asignada tras free)
    };

    Stats stats = service.run(commands);
    ASSERT_EQ(stats.totalAccesses, 0u); // el read fue rechazado antes de llegar al mmu
}
