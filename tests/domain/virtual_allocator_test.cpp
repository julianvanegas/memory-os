#include "test_framework.h"
#include "domain/virtual_allocator.h"

using namespace vmsim::domain;

TEST(allocator_starts_at_zero_and_grows_monotonically) {
    VirtualAllocator allocator;
    auto first = allocator.alloc(PAGE_SIZE * 2);
    ASSERT_TRUE(first.has_value());
    ASSERT_EQ(*first, 0u);

    auto second = allocator.alloc(PAGE_SIZE);
    ASSERT_TRUE(second.has_value());
    ASSERT_EQ(*second, PAGE_SIZE * 2);
}

TEST(allocator_rounds_up_to_page_multiples) {
    VirtualAllocator allocator;
    auto base = allocator.alloc(1); // 1 byte -> ocupa 1 pagina completa
    ASSERT_TRUE(base.has_value());
    ASSERT_TRUE(allocator.isAllocated(0));
    ASSERT_TRUE(allocator.isAllocated(PAGE_SIZE - 1));
    ASSERT_FALSE(allocator.isAllocated(PAGE_SIZE));
}

TEST(allocator_free_marks_range_unallocated) {
    VirtualAllocator allocator;
    auto base = allocator.alloc(PAGE_SIZE);
    ASSERT_TRUE(allocator.isAllocated(*base));
    auto freed = allocator.free(*base);
    ASSERT_TRUE(freed.has_value());
    ASSERT_FALSE(allocator.isAllocated(*base));
}

TEST(allocator_free_with_invalid_address_fails) {
    VirtualAllocator allocator;
    allocator.alloc(PAGE_SIZE);
    auto freed = allocator.free(999999);
    ASSERT_FALSE(freed.has_value());
}

TEST(allocator_handles_maximum_virtual_request_without_overflow) {
    VirtualAllocator allocator;
    auto base = allocator.alloc(UINT32_MAX);
    ASSERT_TRUE(base.has_value());
    ASSERT_TRUE(allocator.isAllocated(UINT32_MAX));

    auto exhausted = allocator.alloc(1);
    ASSERT_FALSE(exhausted.has_value());
}
