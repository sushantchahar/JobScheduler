#include <JobScheduler/JobHandle.h>
#include <JobScheduler/Scheduler.h>

EJobState JobHandle::State() const
{
    auto record = scheduler->FindRecord(id);
    return record.State;
}

bool JobHandle::IsFinished() const
{
    auto record = scheduler->FindRecord(id);
    return record.Outcome.has_value();
}

std::optional<Result<Success>> JobHandle::Outcome() const
{
    auto record = scheduler->FindRecord(id);
    return record.Outcome;
}
