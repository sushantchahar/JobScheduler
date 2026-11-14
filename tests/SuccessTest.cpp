#include <gtest/gtest.h>
#include <JobScheduler/Success.h>
#include <type_traits>

TEST(SuccessTest, SuccessEmpty)
{
    static_assert(std::is_empty_v<Success>);
}