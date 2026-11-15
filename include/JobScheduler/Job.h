#ifndef JOBSCHEDULER_JOB_H
#define JOBSCHEDULER_JOB_H
#include <JobScheduler/EPriority.h>
#include <JobScheduler/JobState.h>
#include <JobScheduler/JobId.h>
#include <JobScheduler/Result.h>
#include <JobScheduler/Success.h>
#include <functional>
#include <string>
#include <optional>

class Job
{
private:
    const JobId Id;
    const std::string Name;
    JobState State;
    const EPriority Priority;
    std::function<Result<Success>()> Callable;
    std::optional<Result<Success>> Outcome;

public:
    Job(JobId id, std::string name, EPriority priority, std::function<Result<Success>()> callable);
    const std::string& GetName() const;
    JobId GetId() const;
    EPriority GetPriority() const;
    EJobState GetState() const;
    [[nodiscard]] bool TryTransition(EJobState NewState);
    Result<Success> Invoke() const;
    void Complete(Result<Success> result);
    const Result<Success>& GetOutcome() const;
};

#endif //JOBSCHEDULER_JOB_H
