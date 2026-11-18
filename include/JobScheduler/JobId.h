#ifndef JOBSCHEDULER_JOBID_H
#define JOBSCHEDULER_JOBID_H
#include <cstdint>
#include <compare>
#include <functional>

class JobId
{
private:
    std::uint64_t Value;

public:
    explicit JobId(std::uint64_t value) : Value(value) {}
    auto operator<=>(const JobId&) const = default;

    friend std::hash<JobId>;
};

template <>
struct std::hash<JobId>
{
    std::size_t operator()(const JobId& id) const noexcept
    {
        return std::hash<std::uint64_t>{}(id.Value);
    }
};

#endif //JOBSCHEDULER_JOBID_H
