#include "application/simulator_service.h"
#include <iostream>

namespace vmsim::application {

using domain::PAGE_SIZE;
using domain::VirtualAddress;

SimulatorService::SimulatorService(domain::PhysicalMemory& physMem, domain::IReplacementPolicy& policy)
    : mmu_(physMem, policy, stats_) {}

domain::Stats SimulatorService::run(const std::vector<Command>& commands) {
    for (const auto& cmd : commands) {
        switch (cmd.type) {
            case CommandType::Alloc: handleAlloc(cmd); break;
            case CommandType::Write: handleWrite(cmd); break;
            case CommandType::Read:  handleRead(cmd); break;
            case CommandType::Free:  handleFree(cmd); break;
        }
    }
    return stats_;
}

void SimulatorService::handleAlloc(const Command& cmd) {
    auto base = allocator_.alloc(cmd.bytes);
    if (!base) {
        std::cerr << "Error (linea " << cmd.lineNumber << "): alloc de " << cmd.bytes
                  << " bytes no pudo reservarse (espacio virtual agotado)\n";
        return;
    }
    std::cout << "alloc -> base virtual = " << *base << "\n";
}

void SimulatorService::handleWrite(const Command& cmd) {
    if (!allocator_.isAllocated(cmd.address)) {
        std::cerr << "Error (linea " << cmd.lineNumber << "): direccion " << cmd.address
                  << " no esta asignada\n";
        return;
    }
    uint8_t* ptr = mmu_.translate(VirtualAddress(cmd.address), /*isWrite=*/true);
    *ptr = cmd.value;
}

void SimulatorService::handleRead(const Command& cmd) {
    if (!allocator_.isAllocated(cmd.address)) {
        std::cerr << "Error (linea " << cmd.lineNumber << "): direccion " << cmd.address
                  << " no esta asignada\n";
        return;
    }
    uint8_t* ptr = mmu_.translate(VirtualAddress(cmd.address), /*isWrite=*/false);
    std::cout << "read " << cmd.address << " -> " << static_cast<int>(*ptr) << "\n";
}

void SimulatorService::handleFree(const Command& cmd) {
    auto allocation = allocator_.free(cmd.address);
    if (!allocation) {
        std::cerr << "Error (linea " << cmd.lineNumber << "): free invalido en direccion "
                  << cmd.address << " (no es base de una asignacion viva)\n";
        return;
    }
    for (uint64_t offset = 0; offset < allocation->size; offset += PAGE_SIZE) {
        mmu_.unmapPage(VirtualAddress(static_cast<uint32_t>(allocation->base + offset)));
    }
}

} // namespace vmsim::application
