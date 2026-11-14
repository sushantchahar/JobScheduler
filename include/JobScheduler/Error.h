#ifndef JOBSCHEDULER_ERROR_H
#define JOBSCHEDULER_ERROR_H
#include <JobScheduler/EErrorCode.h>
#include <string>

struct Error
{
    EErrorCode Code;
    std::string Message;

    Error() = delete;
    Error(EErrorCode code, std::string message);
};

#endif //JOBSCHEDULER_ERROR_H