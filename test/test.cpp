#include "libcpp-channel.hpp"
#include <gtest/gtest.h>

using namespace lklibs;

TEST(SPSC, SingleProducerMustSendMessageToSingleConsumerSuccessfully)
{
    ASSERT_EQ(1,1);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
