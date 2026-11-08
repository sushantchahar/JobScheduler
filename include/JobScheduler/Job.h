#ifndef JOBSCHEDULER_JOB_H
#define JOBSCHEDULER_JOB_H
#include "EPriority.h"
#include <functional>
#include "JobState.h"
#include <string>
#include "JobId.h"

class Job
{
private:
    const JobId Id;
    const std::string Name;
    JobState State;
    const EPriority Priority;
    std::function<void()> Callable;

public:
    Job(JobId id, std::string name, EPriority priority, std::function<void()> callable);
    const std::string& GetName() const;
    JobId GetId() const;
    EPriority GetPriority() const;
    EJobState GetState() const;
    [[nodiscard]] bool TryTransition(EJobState NewState);
};

#endif //JOBSCHEDULER_JOB_H
