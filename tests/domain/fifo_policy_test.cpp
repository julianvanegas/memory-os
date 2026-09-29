#include "test_framework.h"
#include "domain/fifo_policy.h"

using namespace vmsim::domain;

TEST(fifo_evicts_oldest_loaded_frame) {
    FifoPolicy policy;
    policy.onFrameLoaded(1);
    policy.onFrameLoaded(2);
    policy.onFrameLoaded(3);
    ASSERT_EQ(policy.pickVictim(), 1u);
}

TEST(fifo_ignores_access_recency) {
    FifoPolicy policy;
    policy.onFrameLoaded(1);
    policy.onFrameLoaded(2);
    policy.onAccess(1); // no deberia cambiar el orden en FIFO
    ASSERT_EQ(policy.pickVictim(), 1u);
}

TEST(fifo_onFrameFreed_removes_bookkeeping) {
    FifoPolicy policy;
    policy.onFrameLoaded(1);
    policy.onFrameLoaded(2);
    policy.onFrameFreed(1);
    ASSERT_EQ(policy.pickVictim(), 2u);
}
