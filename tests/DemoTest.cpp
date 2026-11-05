/*
 * It is a temporary test just expose following folder structure
*/

#include <gtest/gtest.h>

TEST(TemporaryTest, OnePlusTwo)
{
    ASSERT_TRUE(1 + 2 == 3);
}

TEST(TemporaryTest, CheckingWarningPipeline)
{
    int ID;
    ASSERT_TRUE(1 - 1 == ID);
}
