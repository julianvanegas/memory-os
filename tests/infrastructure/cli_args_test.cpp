#include "test_framework.h"
#include "infrastructure/cli_args.h"
#include <cstdio>
#include <stdexcept>

using namespace vmsim::infrastructure;

namespace {
bool rejectsPhysicalMemory(const char* value) {
    char program[] = "vmsim";
    char input[] = "commands.txt";
    char physicalMemory[64];
    std::snprintf(physicalMemory, sizeof(physicalMemory), "--phys-mem=%s", value);
    char* argv[] = {program, input, physicalMemory};
    try {
        CliArgs::parse(3, argv);
    } catch (const std::runtime_error&) {
        return true;
    }
    return false;
}
} // namespace

TEST(cli_rejects_physical_memory_below_minimum) {
    ASSERT_TRUE(rejectsPhysicalMemory("16K"));
}

TEST(cli_rejects_malformed_physical_memory) {
    ASSERT_TRUE(rejectsPhysicalMemory("4fooK"));
}

TEST(cli_accepts_aligned_minimum_physical_memory) {
    char program[] = "vmsim";
    char input[] = "commands.txt";
    char physicalMemory[] = "--phys-mem=256K";
    char* argv[] = {program, input, physicalMemory};
    auto config = CliArgs::parse(3, argv);
    ASSERT_EQ(config.physMemBytes, 256u * 1024u);
}