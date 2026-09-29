#include "test_framework.h"
#include "domain/physical_memory.h"
#include <stdexcept>

using namespace vmsim::domain;

TEST(physical_memory_reports_correct_frame_count) {
    PhysicalMemory mem(2 * PAGE_SIZE);
    ASSERT_EQ(mem.frameCount(), 2u);
    ASSERT_TRUE(mem.hasFreeFrame());
}

TEST(physical_memory_allocate_and_free_frame) {
    PhysicalMemory mem(1 * PAGE_SIZE);
    uint32_t frame = mem.allocateFrame(42);
    ASSERT_FALSE(mem.hasFreeFrame());
    ASSERT_TRUE(mem.ownerOf(frame).has_value());
    ASSERT_EQ(*mem.ownerOf(frame), 42u);

    mem.freeFrame(frame);
    ASSERT_TRUE(mem.hasFreeFrame());
    ASSERT_FALSE(mem.ownerOf(frame).has_value());
}

TEST(physical_memory_write_and_read_bytes) {
    PhysicalMemory mem(1 * PAGE_SIZE);
    uint32_t frame = mem.allocateFrame(0);
    uint8_t* ptr = mem.pointerTo(frame, 10);
    *ptr = 99;
    ASSERT_EQ(*mem.pointerTo(frame, 10), 99);
}

TEST(physical_memory_rejects_invalid_physical_addresses) {
    PhysicalMemory mem(1 * PAGE_SIZE);
    bool threw = false;
    try {
        mem.pointerTo(1, 0);
    } catch (const std::out_of_range&) {
        threw = true;
    }
    ASSERT_TRUE(threw);

    threw = false;
    try {
        mem.pointerTo(0, PAGE_SIZE);
    } catch (const std::out_of_range&) {
        threw = true;
    }
    ASSERT_TRUE(threw);
}
