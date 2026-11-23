#ifndef JOBSCHEDULER_JOBHANDLE_H
#define JOBSCHEDULER_JOBHANDLE_H
#include <JobScheduler/JobId.h>
#include <JobScheduler/Result.h>
#include <JobScheduler/Success.h>
#include <JobScheduler/EJobState.h>
#include <optional>

class Scheduler;

class JobHandle
{
    friend class Scheduler;
    JobId id;
    const Scheduler* scheduler = nullptr;

    JobHandle(JobId idparam, Scheduler& schedulerparam) : id(idparam), scheduler(&schedulerparam) {}

public:
    const JobId& Id() const { return id; }

    [[nodiscard]] EJobState State() const;
    [[nodiscard]] bool IsFinished() const;
    [[nodiscard]] std::optional<Result<Success>> Outcome() const;
};

#endif //JOBSCHEDULER_JOBHANDLE_H
