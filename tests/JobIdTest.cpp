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

TEST(JobIdTest, EqualIdHashEqually)
{
 JobId id1(1);
 JobId id2(1);
 EXPECT_EQ(id1, id2);
}

TEST(JobIdTest, WorksAsUnorderedMapKey)
{
 std::unordered_map<JobId, int> map;
 JobId id1(5);
 map.emplace(id1, 42);
 JobId id2(5);
 auto a = map.find(id2);
 EXPECT_EQ(a->second, 42);
}

TEST(JobIdTest, DifferentIdsAreDistinctKeys)
{
 JobId id1(5);
 JobId id2(10);
 EXPECT_FALSE(id1 == id2);
}