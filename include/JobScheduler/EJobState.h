#ifndef JOBSCHEDULER_EJOBSTATE_H
#define JOBSCHEDULER_EJOBSTATE_H

enum class EJobState : uint8_t
{
    Created,
    Pending,
    Running,
    Cancelled,
    Failed,
    Succeeded
};

#endif //JOBSCHEDULER_EJOBSTATE_H
