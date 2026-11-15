#include <stdexcept>
#include <JobScheduler/Job.h>
#include <cassert>

Job::Job(JobId id, std::string name, EPriority priority, std::function<Result<Success>()> callable) :
Id(id),
Name(std::move(name)),
Priority(priority),
Callable(std::move(callable))
{
    if (!Callable) {throw std::invalid_argument("Callable cannot be null");}
}

const std::string& Job::GetName() const
{
    return Name;
}

JobId Job::GetId() const
{
    return Id;
}

EPriority Job::GetPriority() const
{
    return Priority;
}

EJobState Job::GetState() const
{
    return State.GetState();
}

bool Job::TryTransition(EJobState NewState)
{
    return State.TryTransition(NewState);
}

Result<Success> Job::Invoke() const
{
    assert(State.GetState() == EJobState::Running);
    return Callable();
}
