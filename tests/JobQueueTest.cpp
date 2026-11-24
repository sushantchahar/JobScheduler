#include <gtest/gtest.h>
#include <JobScheduler/JobQueue.h>
#include <JobScheduler/Job.h>

TEST(JobQueueTest, EmptyQueue)
{
    JobQueue queue;
    ASSERT_TRUE(queue.Empty());
    ASSERT_EQ(queue.Size(), 0);
}

TEST(JobQueueTest, PopOnEmptyQueueReturnsNullopt)
{
    JobQueue queue;
    auto popedjob = queue.TryPop();
    ASSERT_FALSE(popedjob.has_value());
}

TEST(JobQueueTest, SizeTracksPushPop)
{
    JobQueue queue;
    JobId id(1);
    JobId id2(2);
    JobId id3(3);
    std::function<Result<Success>()> callable = []() { return Success{}; };
    Job job(id, "PrintName", EPriority::High, callable);
    Job job1(id2, "PrintReport", EPriority::Low, callable);
    Job job2(id3, "GeneratePDF", EPriority::Normal, callable);

    queue.Push(std::move(job));
    ASSERT_EQ(queue.Size(), 1);
    queue.Push(std::move(job1));
    ASSERT_EQ(queue.Size(), 2);
    queue.Push(std::move(job2));
    ASSERT_EQ(queue.Size(), 3);

    queue.TryPop();
    ASSERT_EQ(queue.Size(), 2);
    queue.TryPop();
    ASSERT_EQ(queue.Size(), 1);
    queue.TryPop();
    ASSERT_EQ(queue.Size(), 0);

    ASSERT_TRUE(queue.Empty());
    ASSERT_EQ(queue.Size(), 0);
}

TEST(JobQueueTest, HigherPriorityPopsFirst)
{
    JobQueue queue;
    JobId id(1);
    JobId id2(2);
    JobId id3(3);
    std::function<Result<Success>()> callable = []() { return Success{}; };
    Job job(id, "PrintName", EPriority::High, callable);
    Job job1(id2, "PrintReport", EPriority::Low, callable);
    Job job2(id3, "GeneratePDF", EPriority::Normal, callable);

    queue.Push(std::move(job));
    queue.Push(std::move(job1));
    queue.Push(std::move(job2));

    std::optional<Job> popedjob = queue.TryPop();
    ASSERT_EQ(popedjob->GetPriority(), EPriority::High);
    popedjob = queue.TryPop();
    ASSERT_EQ(popedjob->GetPriority(), EPriority::Normal);
    popedjob = queue.TryPop();
    ASSERT_EQ(popedjob->GetPriority(), EPriority::Low);
}

TEST(JobQueueTest, EqualPriorityPopsInPushOrder)
{
    JobQueue queue;
    JobId id(1);
    JobId id2(2);
    JobId id3(3);
    std::function<Result<Success>()> callable = []() { return Success{}; };
    Job job(id, "PrintName", EPriority::Normal, callable);
    Job job1(id2, "PrintReport", EPriority::Normal, callable);
    Job job2(id3, "GeneratePDF", EPriority::Normal, callable);

    queue.Push(std::move(job));
    queue.Push(std::move(job1));
    queue.Push(std::move(job2));

    std::optional<Job> poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), id);
    poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), id2);
    poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), id3);
}

TEST(JobQueueTest, MixedPrioritiesKeepFifoInEachLevel)
{
    JobQueue queue;
    JobId id(1);
    JobId id1(2);
    JobId id2(3);
    JobId id3(4);
    JobId id4(5);
    JobId id5(6);
    JobId id6(7);
    JobId id7(8);
    JobId id8(9);
    std::function<Result<Success>()> callable = []() { return Success{}; };
    Job job(id, "PrintName", EPriority::Normal, callable);
    Job job1(id1, "PrintReport", EPriority::Low, callable);
    Job job2(id2, "GeneratePDF", EPriority::Normal, callable);
    Job job3(id3, "GenerateName", EPriority::High, callable);
    Job job4(id4, "GenerateInvoide", EPriority::Low, callable);
    Job job5(id5, "SendEmail", EPriority::Normal, callable);
    Job job6(id6, "SendSms", EPriority::High, callable);
    Job job7(id7, "GenerateImage", EPriority::High, callable);
    Job job8(id8, "GenerateVideo", EPriority::Low, callable);

    queue.Push(std::move(job));
    queue.Push(std::move(job1));
    queue.Push(std::move(job2));
    queue.Push(std::move(job3));
    queue.Push(std::move(job4));
    queue.Push(std::move(job5));
    queue.Push(std::move(job6));
    queue.Push(std::move(job7));
    queue.Push(std::move(job8));

    /*
    High: 4, 7, 8
    Normal: 1, 3, 6
    Low: 2, 5, 9
    */
    std::optional<Job> poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(4));
    ASSERT_EQ(poppedjob->GetPriority(), EPriority::High);
    poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(7));
    ASSERT_EQ(poppedjob->GetPriority(), EPriority::High);
    poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(8));
    ASSERT_EQ(poppedjob->GetPriority(), EPriority::High);

    poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(1));
    ASSERT_EQ(poppedjob->GetPriority(), EPriority::Normal);

    poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(3));
    ASSERT_EQ(poppedjob->GetPriority(), EPriority::Normal);

    poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(6));
    ASSERT_EQ(poppedjob->GetPriority(), EPriority::Normal);

    poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(2));
    ASSERT_EQ(poppedjob->GetPriority(), EPriority::Low);

    poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(5));
    ASSERT_EQ(poppedjob->GetPriority(), EPriority::Low);

    poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(9));
    ASSERT_EQ(poppedjob->GetPriority(), EPriority::Low);
}

TEST(JobQueueTest, PushAfterPopKeepsOrder)
{
    JobQueue queue;
    JobId id(1);
    JobId id1(2);
    JobId id2(3);
    JobId id3(4);
    JobId id4(5);
    std::function<Result<Success>()> callable = []() { return Success{}; };
    Job job(id, "PrintName", EPriority::Normal, callable);
    Job job1(id1, "PrintReport", EPriority::Low, callable);
    Job job2(id2, "GeneratePDF", EPriority::Normal, callable);
    Job job3(id3, "GenerateName", EPriority::High, callable);
    Job job4(id4, "GenerateInvoide", EPriority::Normal, callable);

    // Expected order after all push: 4, 1, 3, 5, 2
    queue.Push(std::move(job));
    queue.Push(std::move(job1));
    queue.Push(std::move(job2));
    queue.Push(std::move(job3));

    std::optional<Job> poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(4));

    queue.Push(std::move(job4));

    poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(1));
    poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(3));
    poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(5));
    poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(2));

    ASSERT_TRUE(queue.Empty());
}

TEST(JobQueueTest, PushMarksJobPending)
{
    JobQueue queue;
    JobId id(1);

    std::function<Result<Success>()> callable = []() { return Success{}; };
    Job job(id, "PrintName", EPriority::Normal, callable);

    ASSERT_EQ(job.GetState(), EJobState::Created);
    queue.Push(std::move(job));

    std::optional<Job> poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetState(), EJobState::Pending);
}

TEST(JobQueueTest, SameSequenceFifoOrderFollowedAfterPoppedAndPush)
{
    JobQueue queue;
    JobId id(1);
    JobId id2(2);
    JobId id3(3);
    JobId id4(4);
    std::function<Result<Success>()> callable = []() { return Success{}; };
    Job job(id, "PrintName", EPriority::Normal, callable);
    Job job1(id2, "PrintReport", EPriority::Normal, callable);
    Job job2(id3, "GeneratePDF", EPriority::Normal, callable);
    Job job3(id4, "GenerateName", EPriority::Normal, callable);

    queue.Push(std::move(job));
    ASSERT_EQ(queue.Size(), 1);
    queue.Push(std::move(job1));
    ASSERT_EQ(queue.Size(), 2);
    queue.Push(std::move(job2));
    ASSERT_EQ(queue.Size(), 3);

    std::optional<Job> poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(1));
    poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(2));

    queue.Push(std::move(job3));

    poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(3));
    poppedjob = queue.TryPop();
    ASSERT_EQ(poppedjob->GetId(), JobId(4));
}