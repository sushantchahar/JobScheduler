#ifndef JOBSCHEDULER_EERRORCODE_H
#define JOBSCHEDULER_EERRORCODE_H
#include <cstdint>

enum class EErrorCode : std::uint8_t
{
    JobFailed,
    Exception,
    InvalidArgument
};

#endif //JOBSCHEDULER_EERRORCODE_H