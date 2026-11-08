#ifndef JOBSCHEDULER_JOBID_H
#define JOBSCHEDULER_JOBID_H
#include <cstdint>
#include <compare>

class JobId
{
private:
    std::uint64_t Value;

public:
    explicit JobId(std::uint64_t value) : Value(value) {}
    auto operator<=>(const JobId&) const = default;
};

#endif //JOBSCHEDULER_JOBID_H
