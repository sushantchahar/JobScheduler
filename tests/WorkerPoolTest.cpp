#include <gtest/gtest.h>
#include <JobScheduler/WorkerPool.h>
#include <JobScheduler/Job.h>
#include <JobScheduler/JobId.h>
#include <JobScheduler/EPriority.h>
#include <latch>
#include <atomic>
#include <functional>

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
        [&finished, &done](JobId, Result<Success>) {finished++; done.count_down();}
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
        WorkerPool pool(queue, 4, [](JobId) {}, [](JobId, Result<Success>) {});
    }
}