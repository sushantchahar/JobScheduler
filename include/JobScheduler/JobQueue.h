#ifndef JOBSCHEDULER_JOBQUEUE_H
#define JOBSCHEDULER_JOBQUEUE_H
#include <cstdint>
#include <vector>
#include <JobScheduler/Job.h>
#include <optional>
#include <cstddef>
#include <mutex>
#include <condition_variable>
#include <stop_token>

struct Entry
{
    Job job;
    uint64_t sequence;

    Entry(Job job, uint64_t sequence) : job(std::move(job)), sequence(sequence) {}
};

struct EntryCompare
{
    bool operator()(const Entry& lhs, const Entry& rhs) const
    {
        if (lhs.job.GetPriority() < rhs.job.GetPriority()) return true;
        else if (lhs.job.GetPriority() > rhs.job.GetPriority()) return false;
        else return lhs.sequence > rhs.sequence;
    }
};

class JobQueue
{
private:
    std::vector<Entry> Heap;
    uint64_t NextSequence = 0;
    mutable std::mutex QueueMutex;
    std::condition_variable_any QueueCondition;

    std::optional<Job> PopLocked();

public:
    void Push(Job&& job);
    std::optional<Job> TryPop();
    std::optional<Job> Pop();
    std::optional<Job> Pop(std::stop_token token);
    [[nodiscard]] std::size_t Size() const;
    [[nodiscard]] bool Empty() const;
};


#endif //JOBSCHEDULER_JOBQUEUE_H