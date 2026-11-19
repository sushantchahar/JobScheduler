#include <gtest/gtest.h>
#include <JobScheduler/Scheduler.h>
#include <string>
#include <functional>
#include <JobScheduler/Result.h>
#include <JobScheduler/Success.h>

TEST(SchedulerTest, ValidSubmitReturnsHandle)
{
    std::function<Result<Success>()> callable = [] () -> Result<Success>
    {
        return Success{};
    };
    SubmitSpecs specs{"PrintName", EPriority::Low, callable};

    Scheduler scheduler;
    auto result = scheduler.Submit(specs);
    EXPECT_TRUE(result.IsOk());
}

TEST(SchedulerTest, EmptyCallableIsRejected)
{
    std::function<Result<Success>()> callable;
    SubmitSpecs specs{"PrintName", EPriority::Low, callable};
    Scheduler scheduler;
    auto result = scheduler.Submit(specs);
    ASSERT_FALSE(result.IsOk());
    EXPECT_EQ(result.GetError().Code, EErrorCode::InvalidArgument);
}

TEST(SchedulerTest, RejectedSubmitDoesnotConsumeId)
{
    std::function<Result<Success>()> callable;
    std::function<Result<Success>()> callable2 = [] () -> Result<Success>
    {
        return Success{};
    };
    SubmitSpecs specs{"PrintName", EPriority::Low, callable};
    SubmitSpecs specs2{"GeneratePDF", EPriority::High, callable2};
    Scheduler scheduler;
    auto result = scheduler.Submit(specs);
    auto result2 = scheduler.Submit(specs2);
    ASSERT_FALSE(result.IsOk());
    ASSERT_TRUE(result2.IsOk());

    auto id = result2.GetValue().Id();
    ASSERT_EQ(id, JobId(0));
}

TEST(SchedulerTest, SubmitAssignsUniqueIds)
{
    std::function<Result<Success>()> callable = [] () -> Result<Success>
    {
        return Success{};
    };
    SubmitSpecs specs{"PrintName", EPriority::Low, callable};
    SubmitSpecs specs2{"GeneratePDF", EPriority::High, callable};
    Scheduler scheduler;
    auto result = scheduler.Submit(specs);
    auto result2 = scheduler.Submit(specs2);

    ASSERT_TRUE(result.IsOk());
    ASSERT_TRUE(result2.IsOk());

    ASSERT_NE(result.GetValue().Id(), result2.GetValue().Id());
}