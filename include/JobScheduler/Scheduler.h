#ifndef JOBSCHEDULER_SCHEDULER_H
#define JOBSCHEDULER_SCHEDULER_H
#include <JobScheduler/JobQueue.h>
#include <cstdint>
#include <JobScheduler/EJobState.h>
#include <optional>
#include <JobScheduler/Result.h>
#include <JobScheduler/Success.h>
#include <unordered_map>
#include <JobScheduler/JobId.h>
#include <JobScheduler/JobHandle.h>
#include <functional>
#include <string>

struct SubmitSpecs
{
    std::string Name;
    EPriority Priority = EPriority::Normal;
    std::function<Result<Success>()> Callable;
};

class Scheduler
{
private:
    struct Record
    {
        EJobState State = EJobState::Pending;
        std::optional<Result<Success>> Outcome;

        Record() = default;
        Record(EJobState state, Result<Success> outcome) : State(state), Outcome(outcome) {}
    };

    JobQueue jobQueue;
    uint64_t JobIdCounter = 0;
    std::unordered_map<JobId, Record> records;

public:
    Scheduler() = default;
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;
    Scheduler(Scheduler&&) = delete;
    Scheduler& operator=(Scheduler&&) = delete;

    Result<JobHandle> Submit(SubmitSpecs specs);
};

#endif //JOBSCHEDULER_SCHEDULER_H
