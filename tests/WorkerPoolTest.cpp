#include <gtest/gtest.h>
#include <JobScheduler/WorkerPool.h>
#include <JobScheduler/Job.h>
#include <JobScheduler/JobId.h>
#include <JobScheduler/EPriority.h>
#include <JobScheduler/Result.h>
#include <JobScheduler/Success.h>
#include <JobScheduler/EJobState.h>
#include <latch>
#include <atomic>
#include <functional>
#include <thread>
#include <chrono>

TEST(WorkerPoolTest, RunAllJobsOnWorkers)
{
    JobQueue queue;
    const int JOBS = 100;
    std::latch done(JOBS);
    std::atomic<int> started{0};
    std::atomic<int> finished{0};

    auto callable = []() {return Success();};
    for (int i = 1; i <= JOBS; i++)
    {
        JobId id(i * JOBS);
        Job job(id, "PrintName", EPriority::Normal, callable);

        queue.Push(std::move(job));
    }

    {
        WorkerPool pool(queue,
        4,
        [&started](JobId) {started++;},
        [&finished, &done](JobId, EJobState, Result<Success>) {finished++; done.count_down();}
        );

        done.wait();
    }

    EXPECT_EQ(started, JOBS);
    EXPECT_EQ(finished, JOBS);
    EXPECT_EQ(queue.Empty(), true);
}

TEST(WorkerPoolTest, DestroysCleanlyWhenIdle)
{
    JobQueue queue;
    {
        WorkerPool pool(queue, 4, [](JobId) {}, [](JobId, EJobState, Result<Success>) {});
    }
}

TEST(WorkerPoolTest, StopIsIdempotent)
{
    JobQueue queue;
    WorkerPool workerPool
    (queue,
    4,
    [](JobId) {},
    [](JobId, EJobState, Result<Success>) {}
    );

    workerPool.Stop();
    workerPool.Stop();
    SUCCEED();
}

TEST(WorkerPoolTest, StopFinishesRunningJobAndLeavesPendingJobs)
{
    JobQueue queue;
    std::latch started(1);
    std::atomic<bool> release{false};
    std::atomic<int> pendingRan{0};
    std::atomic<int> finished{0};

    Job blocker(JobId(1), "Blocker", EPriority::High, [&]() -> Result<Success>
    {
        started.count_down();
        while (!release)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        return Success{};
    });
    queue.Push(std::move(blocker));

    for (int i = 2; i <= 3; ++i)
    {
        Job pending(JobId(i), "Pending", EPriority::Low, [&pendingRan]() -> Result<Success>
        {
            pendingRan++;
            return Success{};
        });
        queue.Push(std::move(pending));
    }

    {
        WorkerPool pool(queue,
        1,
        [](JobId) {},
        [&finished](JobId, EJobState, Result<Success>) { finished++; }
        );

        started.wait();
        std::jthread stopper([&pool]() { pool.Stop(); });
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        release = true;
    }

    EXPECT_EQ(finished, 1);
    EXPECT_EQ(pendingRan, 0);
    EXPECT_EQ(queue.Size(), 2u);
}