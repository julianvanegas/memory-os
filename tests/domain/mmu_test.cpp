#include "test_framework.h"
#include "domain/fifo_policy.h"
#include "domain/mmu.h"

using namespace vmsim::domain;

TEST(mmu_first_access_causes_page_fault) {
    PhysicalMemory mem(2 * PAGE_SIZE);
    FifoPolicy policy;
    Stats stats;
    Mmu mmu(mem, policy, stats);

    mmu.translate(VirtualAddress(0), false);
    ASSERT_EQ(stats.pageFaults, 1u);
    ASSERT_EQ(stats.totalAccesses, 1u);
}

TEST(mmu_second_access_to_same_page_is_a_hit) {
    PhysicalMemory mem(2 * PAGE_SIZE);
    FifoPolicy policy;
    Stats stats;
    Mmu mmu(mem, policy, stats);

    mmu.translate(VirtualAddress(0), false);
    mmu.translate(VirtualAddress(10), false); // misma pagina, offset distinto
    ASSERT_EQ(stats.pageFaults, 1u);
    ASSERT_EQ(stats.totalAccesses, 2u);
}

TEST(mmu_triggers_replacement_when_out_of_frames) {
    PhysicalMemory mem(2 * PAGE_SIZE); // solo 2 frames
    FifoPolicy policy;
    Stats stats;
    Mmu mmu(mem, policy, stats);

    mmu.translate(VirtualAddress(0 * PAGE_SIZE), false);
    mmu.translate(VirtualAddress(1 * PAGE_SIZE), false);
    mmu.translate(VirtualAddress(2 * PAGE_SIZE), false); // fuerza reemplazo

    ASSERT_EQ(stats.pageFaults, 3u);
    ASSERT_EQ(stats.replacements, 1u);
}

TEST(mmu_write_then_read_returns_same_value) {
    PhysicalMemory mem(2 * PAGE_SIZE);
    FifoPolicy policy;
    Stats stats;
    Mmu mmu(mem, policy, stats);

    uint8_t* w = mmu.translate(VirtualAddress(0), true);
    *w = 77;
    uint8_t* r = mmu.translate(VirtualAddress(0), false);
    ASSERT_EQ(*r, 77);
}

TEST(mmu_unmap_frees_frame_for_reuse) {
    PhysicalMemory mem(1 * PAGE_SIZE); // 1 solo frame
    FifoPolicy policy;
    Stats stats;
    Mmu mmu(mem, policy, stats);

    mmu.translate(VirtualAddress(0), false);
    mmu.unmapPage(VirtualAddress(0));
    ASSERT_TRUE(mem.hasFreeFrame());

    mmu.translate(VirtualAddress(PAGE_SIZE), false); // no deberia forzar reemplazo
    ASSERT_EQ(stats.replacements, 0u);
}
