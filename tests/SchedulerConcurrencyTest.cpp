#include <gtest/gtest.h>
#include <JobScheduler/Scheduler.h>
#include <JobScheduler/JobHandle.h>
#include <JobScheduler/JobId.h>
#include <atomic>
#include <vector>
#include <chrono>
#include <thread>
#include <unordered_set>

TEST(SchedulerConcurrencyTest, WorkersRunAllSubmittedJobs)
{
    std::atomic<int> ran{0};
    Scheduler scheduler(4);
    const int JOBS = 100;
    std::vector<JobHandle> handles;

    for (int i = 1; i <= JOBS; i++)
    {
        auto callable = [&ran]() -> Result<Success>
        {
            ran++;
            return Success{};
        };

        SubmitSpecs specs("PrintSomething", EPriority::Low, callable);
        auto result = scheduler.Submit(specs);
        ASSERT_TRUE(result.IsOk());
        handles.push_back(result.GetValue());
    }

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    for (const auto& handle : handles)
    {
        while (!handle.IsFinished() && std::chrono::steady_clock::now() < deadline)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        ASSERT_TRUE(handle.IsFinished());
        EXPECT_TRUE(handle.State() == EJobState::Succeeded);
    }

    EXPECT_EQ(ran, JOBS);
}

TEST(SchedulerConcurrencyTest, ConcurrentSubmitGivesUniqueIds)
{
    constexpr std::size_t THREADS = 4;
    constexpr std::size_t PER_THREAD = 100;
    constexpr std::size_t TOTAL = THREADS * PER_THREAD;

    std::atomic<int> ran{0};
    Scheduler scheduler(4);
    std::vector<std::vector<JobHandle>> perThread(THREADS);

    {
        std::vector<std::jthread> submitters;
        for (std::size_t t = 0; t < THREADS; ++t)
        {
            submitters.emplace_back([&scheduler, &ran, &mine = perThread[t]]()
            {
                for (std::size_t i = 0; i < PER_THREAD; ++i)
                {
                    SubmitSpecs specs("PrintSomething", EPriority::Low,
                                      [&ran]() -> Result<Success>
                                      {
                                          ran++;
                                          return Success{};
                                      });
                    auto result = scheduler.Submit(specs);
                    ASSERT_TRUE(result.IsOk());
                    mine.push_back(result.GetValue());
                }
            });
        }
    }

    std::vector<JobHandle> all;
    for (const auto& handles : perThread)
    {
        all.insert(all.end(), handles.begin(), handles.end());
    }
    ASSERT_EQ(all.size(), TOTAL);

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    for (const auto& handle : all)
    {
        while (!handle.IsFinished() && std::chrono::steady_clock::now() < deadline)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        ASSERT_TRUE(handle.IsFinished());
        EXPECT_TRUE(handle.State() == EJobState::Succeeded);
    }

    std::unordered_set<JobId> ids;
    for (const auto& handle : all)
    {
        ids.insert(handle.Id());
    }
    EXPECT_EQ(ids.size(), TOTAL);            // no duplicate ids
    EXPECT_EQ(ran, static_cast<int>(TOTAL));
}