#ifndef JOBSCHEDULER_JOB_H
#define JOBSCHEDULER_JOB_H
#include <JobScheduler/EPriority.h>
#include <JobScheduler/JobState.h>
#include <JobScheduler/JobId.h>
#include <JobScheduler/Result.h>
#include <JobScheduler/Success.h>
#include <functional>
#include <string>

class Job
{
private:
    const JobId Id;
    const std::string Name;
    JobState State;
    const EPriority Priority;
    std::function<Result<Success>()> Callable;

public:
    Job(JobId id, std::string name, EPriority priority, std::function<Result<Success>()> callable);
    const std::string& GetName() const;
    JobId GetId() const;
    EPriority GetPriority() const;
    EJobState GetState() const;
    [[nodiscard]] bool TryTransition(EJobState NewState);
};

#endif //JOBSCHEDULER_JOB_H
