#ifndef JOBSCHEDULER_JOBSTATE_H
#define JOBSCHEDULER_JOBSTATE_H
#include "EJobState.h"

class JobState
{
private:
    EJobState State = EJobState::Created;

    // Methods
    bool IsValidTransition(EJobState From, EJobState To) const;

public:
    EJobState GetState() const;
    bool operator==(const JobState&) const = default;
    [[nodiscard]] bool TryTransition(EJobState NewState);
};

#endif //JOBSCHEDULER_JOBSTATE_H
