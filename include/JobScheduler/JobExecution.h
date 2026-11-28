#ifndef JOBSCHEDULER_JOBEXECUTION_H
#define JOBSCHEDULER_JOBEXECUTION_H
#include <JobScheduler/Job.h>
#include <JobScheduler/Result.h>
#include <JobScheduler/Success.h>

namespace JobExecuter
{
    Result<Success> RunJobSafely(Job& job);
}

#endif //JOBSCHEDULER_JOBEXECUTION_H
