/*
 Tests written for validating the implementation and working of JobId
*/

#include <gtest/gtest.h>
#include <JobScheduler/JobId.h>

TEST(JobIdTest, SameIdAreEqual)
{
 EXPECT_EQ(JobId(5), JobId(5));
}

TEST(JobIdTest, DifferentIdAreNotEqual)
{
 EXPECT_NE(JobId(5), JobId(6));
}

TEST(JobIdTest, SmallerIdComparesLess)
{
 EXPECT_LT(JobId(5), JobId(6));
}

TEST(JobIdTest, GreaterIdComparesGreater)
{
 EXPECT_GT(JobId(5), JobId(3));
}

TEST(JobIdTest, GreaterEqualIdComparesGreaterOrEqual)
{
 EXPECT_GE(JobId(7), JobId(6));
}

TEST(JobIdTest, LesserEqualIdComparesLesserOrEqual)
{
 EXPECT_LE(JobId(3), JobId(6));
}

TEST(JobIdTest, ZeroIsSmallerThanOne)
{
 EXPECT_LT(JobId(0), JobId(1));
}

TEST(JobIdTest, SmallestValidId)
{
 constexpr std::uint64_t MinValue = std::numeric_limits<std::uint64_t>::min();
 EXPECT_GE(JobId(MinValue), JobId(0));
}