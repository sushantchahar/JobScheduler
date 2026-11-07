#include <JobScheduler/JobState.h>

bool JobState::IsValidTransition(EJobState From, EJobState To) const
{
    return (From == EJobState::Created && To == EJobState::Pending) ||
            (From == EJobState::Pending && To == EJobState::Running) ||
            (From == EJobState::Pending && To == EJobState::Cancelled) ||
            (From == EJobState::Running && To == EJobState::Failed) ||
            (From == EJobState::Running && To == EJobState::Succeeded) ||
            (From == EJobState::Running && To == EJobState::Cancelled);
}

EJobState JobState::GetState() const
{
    return State;
}

bool JobState::TryTransition(EJobState NewState)
{
    if (!IsValidTransition(State, NewState)) return false;
    State = NewState;
    return true;
}
