/*
 Test for checking the smaller, greater or equal status of all priorities
*/

#include <gtest/gtest.h>
#include <JobScheduler/EPriority.h>

TEST(PriorityTest, ChecksSmallerEqualGreaterPriorityToEachOther)
{
    EXPECT_LT(EPriority::Low, EPriority::Normal);
    EXPECT_LT(EPriority::Low, EPriority::High);
    EXPECT_LT(EPriority::Normal, EPriority::High);
    EXPECT_GT(EPriority::Normal, EPriority::Low);
    EXPECT_GT(EPriority::High, EPriority::Low);
    EXPECT_GT(EPriority::High, EPriority::Normal);
    EXPECT_EQ(EPriority::High, EPriority::High);
    EXPECT_EQ(EPriority::Normal, EPriority::Normal);
    EXPECT_EQ(EPriority::Low, EPriority::Low);
}
