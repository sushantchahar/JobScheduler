#include <JobScheduler/Error.h>
#include <utility>

Error::Error(EErrorCode code, std::string message) :
Code(code),
Message(std::move(message))
{}
