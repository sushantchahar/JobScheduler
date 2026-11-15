#ifndef JOBSCHEDULER_RESULT_H
#define JOBSCHEDULER_RESULT_H
#include <variant>
#include <JobScheduler/Error.h>
#include <utility>
#include <type_traits>
#include <cassert>

template <typename T> class Result
{
    static_assert(!std::is_same_v<T, Error>);
private:
    std::variant<T, Error> Data;

public:
    Result() = delete;
    Result(T value) : Data(std::move(value)) {}
    Result(Error error) : Data(std::move(error)) {}

    bool IsOk() const
    {
        return !std::holds_alternative<Error>(Data);
    }

    const T& GetValue() const
    {
        assert(IsOk());
        return std::get<T>(Data);
    }

    const Error& GetError() const
    {
        assert(!IsOk());
        return std::get<Error>(Data);
    }
};

#endif //JOBSCHEDULER_RESULT_H
