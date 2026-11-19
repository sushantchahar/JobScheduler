#ifndef JOBSCHEDULER_JOBHANDLE_H
#define JOBSCHEDULER_JOBHANDLE_H
#include <JobScheduler/JobId.h>

class Scheduler;

class JobHandle
{
    friend class Scheduler;
    JobId id;
    Scheduler* scheduler = nullptr;

    JobHandle(JobId idparam, Scheduler& schedulerparam) : id(idparam), scheduler(&schedulerparam) {}

public:
    const JobId& Id() const { return id; }
};

#endif //JOBSCHEDULER_JOBHANDLE_H
