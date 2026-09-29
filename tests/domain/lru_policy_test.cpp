#include "test_framework.h"
#include "domain/lru_policy.h"

using namespace vmsim::domain;

TEST(lru_evicts_least_recently_used) {
    LruPolicy policy;
    policy.onFrameLoaded(1);
    policy.onFrameLoaded(2);
    policy.onFrameLoaded(3);
    policy.onAccess(1); // 1 pasa a ser el mas reciente
    ASSERT_EQ(policy.pickVictim(), 2u);
}

TEST(lru_reduces_to_fifo_without_access_calls) {
    LruPolicy policy;
    policy.onFrameLoaded(1);
    policy.onFrameLoaded(2);
    ASSERT_EQ(policy.pickVictim(), 1u);
}

TEST(lru_onFrameFreed_removes_bookkeeping) {
    LruPolicy policy;
    policy.onFrameLoaded(1);
    policy.onFrameLoaded(2);
    policy.onFrameFreed(1);
    ASSERT_EQ(policy.pickVictim(), 2u);
}
