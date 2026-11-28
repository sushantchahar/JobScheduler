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
#include <mutex>
#include <JobScheduler/WorkerPool.h>

struct SubmitSpecs
{
    std::string Name;
    EPriority Priority = EPriority::Normal;
    std::function<Result<Success>()> Callable;
};

class Scheduler
{
    friend class JobHandle;
private:
    struct Record
    {
        EJobState State = EJobState::Pending;
        std::optional<Result<Success>> Outcome;

        Record() = default;
        Record(EJobState state, Result<Success> outcome) : State(state), Outcome(outcome) {}
    };

    JobQueue jobQueue;
    // SchedulerMutex will guard the idcounter and records
    uint64_t JobIdCounter = 0;
    std::unordered_map<JobId, Record> records;
    mutable std::mutex SchedulerMutex;
    bool ShuttingDown = false;
    std::mutex ShutDownMutex;
    WorkerPool workerPool;

    [[nodiscard]] Record FindRecord(JobId id) const;
    void OnJobStarted(JobId id);
    void OnJobFinished(JobId id, EJobState state, Result<Success> result);

public:
    explicit Scheduler(std::size_t workerscount = 0);
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;
    Scheduler(Scheduler&&) = delete;
    Scheduler& operator=(Scheduler&&) = delete;
    bool RunNext();
    size_t RunAll();
    ~Scheduler();
    void ShutDown();

    Result<JobHandle> Submit(SubmitSpecs specs);
};

#endif //JOBSCHEDULER_SCHEDULER_H
