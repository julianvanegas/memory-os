#include "test_framework.h"
#include "domain/page_table.h"

using namespace vmsim::domain;

TEST(page_table_creates_level2_lazily) {
    PageTable table;
    ASSERT_FALSE(table.hasLevel2(0));
    PTE& entry = table.entryFor(VirtualAddress(0));
    ASSERT_TRUE(table.hasLevel2(0));
    ASSERT_FALSE(entry.valid);
}

TEST(page_table_returns_same_entry_for_same_address) {
    PageTable table;
    PTE& a = table.entryFor(VirtualAddress(4096));
    a.valid = true;
    a.frame = 7;
    PTE& b = table.entryFor(VirtualAddress(4096));
    ASSERT_TRUE(b.valid);
    ASSERT_EQ(b.frame, 7u);
}

TEST(page_table_distinguishes_different_pt1_indices) {
    PageTable table;
    uint32_t addrA = 0;               // pt1 = 0
    uint32_t addrB = (1u << 22);       // pt1 = 1
    PTE& a = table.entryFor(VirtualAddress(addrA));
    a.valid = true;
    PTE& b = table.entryFor(VirtualAddress(addrB));
    ASSERT_FALSE(b.valid);
}
