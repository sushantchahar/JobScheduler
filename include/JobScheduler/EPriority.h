#ifndef JOBSCHEDULER_EPRIORITY_H
#define JOBSCHEDULER_EPRIORITY_H
#include <cstdint>

enum class EPriority : uint8_t
{
    Low,
    Normal,
    High
};

#endif //JOBSCHEDULER_EPRIORITY_H
