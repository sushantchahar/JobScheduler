#include <gtest/gtest.h>
#include <JobScheduler/Scheduler.h>
#include <string>
#include <functional>
#include <JobScheduler/Result.h>
#include <JobScheduler/Success.h>
#include <stdexcept>
#include <string>

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

TEST(SchedulerTest, HigherPriorityRunsFirst)
{
    std::vector<std::string> order;
    std::function<Result<Success>()> callable = [&order] () -> Result<Success>
    {
        order.push_back("FirstName");
        return Success{};
    };

    std::function<Result<Success>()> callable2 = [&order] () -> Result<Success>
    {
        order.push_back("LastName");
        return Success{};
    };

    SubmitSpecs specs{"PrintFirstName", EPriority::Low, callable};
    SubmitSpecs specs2{"PrintSurname", EPriority::High, callable2};

    Scheduler scheduler;

    auto result = scheduler.Submit(specs);
    auto result2 = scheduler.Submit(specs2);

    EXPECT_TRUE(result.IsOk());
    EXPECT_TRUE(result2.IsOk());

    EXPECT_TRUE(scheduler.RunNext());
    EXPECT_TRUE(scheduler.RunNext());

    ASSERT_EQ(order.size(), 2u);
    EXPECT_EQ(order[0], "LastName");
    EXPECT_EQ(order[1], "FirstName");
}

TEST(SchedulerTest, EqualPriorityRunsInSubmitOrder)
{
    std::vector<std::string> order;
    std::function<Result<Success>()> callable1 = [&order] () -> Result<Success>
    {
        order.push_back("A");
        return Success{};
    };

    std::function<Result<Success>()> callable2 = [&order] () -> Result<Success>
    {
        order.push_back("B");
        return Success{};
    };

    std::function<Result<Success>()> callable3 = [&order] () -> Result<Success>
    {
        order.push_back("C");
        return Success{};
    };

    SubmitSpecs specs1{"PrintFirstName", EPriority::Low, callable1};
    SubmitSpecs specs2{"PrintSurname", EPriority::Low, callable2};
    SubmitSpecs specs3{"PrintName", EPriority::Low, callable3};

    Scheduler scheduler;

    auto result1 = scheduler.Submit(specs1);
    auto result2 = scheduler.Submit(specs2);
    auto result3 = scheduler.Submit(specs3);

    EXPECT_TRUE(result1.IsOk());
    EXPECT_TRUE(result2.IsOk());
    EXPECT_TRUE(result3.IsOk());

    EXPECT_TRUE(scheduler.RunNext());
    EXPECT_TRUE(scheduler.RunNext());
    EXPECT_TRUE(scheduler.RunNext());

    ASSERT_EQ(order.size(), 3u);
    EXPECT_EQ(order[0], "A");
    EXPECT_EQ(order[1], "B");
    EXPECT_EQ(order[2], "C");
}

TEST(SchedulerTest, FailedJobsDoNotCrashRunNext)
{
    auto callable1 = [] () -> Result<Success> { return Error{EErrorCode::JobFailed, "failed"}; };
    auto callable2 = [] () -> Result<Success> { throw std::runtime_error("boom"); };
    auto callable3 = [] () -> Result<Success> {throw 42;};

    Scheduler scheduler;

    SubmitSpecs specs1{"First", EPriority::Low, callable1};
    SubmitSpecs specs2{"Second", EPriority::Low, callable2};
    SubmitSpecs specs3{"Third", EPriority::Low, callable3};

    auto result1 = scheduler.Submit(specs1);
    auto result2 = scheduler.Submit(specs2);
    auto result3 = scheduler.Submit(specs3);

    ASSERT_TRUE(result1.IsOk());
    ASSERT_TRUE(result2.IsOk());
    ASSERT_TRUE(result3.IsOk());

    EXPECT_TRUE(scheduler.RunNext());
    EXPECT_TRUE(scheduler.RunNext());
    EXPECT_TRUE(scheduler.RunNext());

    EXPECT_FALSE(scheduler.RunNext());
}

TEST(SchedulerTest, RunNextOnEmptyQueueReturnsFalse)
{
    Scheduler scheduler;

    EXPECT_FALSE(scheduler.RunNext());
    EXPECT_FALSE(scheduler.RunNext());
}

TEST(SchedulerTest, NewJobStartsPending)
{
    Scheduler scheduler;
    std::function<Result<Success>()> callable1 = [] () -> Result<Success> {return Success{};};

    SubmitSpecs specs1{"PrintName", EPriority::Low, callable1};

    auto result = scheduler.Submit(specs1);
    ASSERT_TRUE(result.IsOk());

    auto handle = result.GetValue();
    EXPECT_TRUE(handle.State() == EJobState::Pending);
    EXPECT_FALSE(handle.IsFinished());
    EXPECT_FALSE(handle.Outcome().has_value());
}

TEST(SchedulerTest, SucceedJobFinishedWithOk)
{
    Scheduler scheduler;
    std::function<Result<Success>()> callable1 = [] () -> Result<Success> {return Success{};};

    SubmitSpecs specs1{"PrintName", EPriority::Low, callable1};

    auto result = scheduler.Submit(specs1);
    ASSERT_TRUE(result.IsOk());

    ASSERT_TRUE(scheduler.RunNext());

    auto handle = result.GetValue();
    EXPECT_EQ(handle.State(), EJobState::Succeeded);
    EXPECT_TRUE(handle.IsFinished());
    EXPECT_TRUE(handle.Outcome().has_value());
    EXPECT_TRUE(handle.Outcome()->IsOk());
}

TEST(SchedulerTest, FailedJobsReportTheirOutcome)
{
    Scheduler scheduler;
    std::function<Result<Success>()> callable1 = [] () -> Result<Success>
    {return Error{EErrorCode::JobFailed, "failed to print name. name is empty"};};
    std::function<Result<Success>()> callable2 = [] () -> Result<Success>
    {throw std::runtime_error{"boom"};};
    std::function<Result<Success>()> callable3 = [] () -> Result<Success> {throw 56;};

    SubmitSpecs specs1{"PrintName", EPriority::Low, callable1};
    SubmitSpecs specs2{"PrintDescription", EPriority::Low, callable2};
    SubmitSpecs specs3{"PrintSuggestions", EPriority::Low, callable3};

    auto result1 = scheduler.Submit(specs1);
    auto result2 = scheduler.Submit(specs2);
    auto result3 = scheduler.Submit(specs3);
    ASSERT_TRUE(result1.IsOk());
    ASSERT_TRUE(result2.IsOk());
    ASSERT_TRUE(result3.IsOk());

    ASSERT_TRUE(scheduler.RunNext());
    ASSERT_TRUE(scheduler.RunNext());
    ASSERT_TRUE(scheduler.RunNext());

    auto handle1 = result1.GetValue();
    auto handle2 = result2.GetValue();
    auto handle3 = result3.GetValue();

    auto outcome = handle1.Outcome();
    ASSERT_TRUE(outcome.has_value());
    ASSERT_FALSE(outcome->IsOk());
    EXPECT_EQ(outcome->GetError().Code, EErrorCode::JobFailed);
    EXPECT_TRUE(handle1.IsFinished());
    EXPECT_EQ(handle1.State(), EJobState::Failed);

    auto outcome2 = handle2.Outcome();
    ASSERT_TRUE(outcome2.has_value());
    ASSERT_FALSE(outcome2->IsOk());
    EXPECT_EQ(outcome2->GetError().Code, EErrorCode::Exception);
    EXPECT_TRUE(handle2.IsFinished());
    EXPECT_EQ(handle2.State(), EJobState::Failed);

    auto outcome3 = handle3.Outcome();
    ASSERT_TRUE(outcome3.has_value());
    ASSERT_FALSE(outcome3->IsOk());
    EXPECT_EQ(outcome3->GetError().Code, EErrorCode::Exception);
    EXPECT_TRUE(handle3.IsFinished());
    EXPECT_EQ(handle3.State(), EJobState::Failed);

    EXPECT_FALSE(scheduler.RunNext());
}