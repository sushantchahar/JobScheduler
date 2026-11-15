#include <gtest/gtest.h>
#include <JobScheduler/Error.h>
#include <JobScheduler/EErrorCode.h>
#include <type_traits>

TEST(ErrorTest, StoresMessageAndCode)
{
    Error error{EErrorCode::JobFailed, "File not found"};
    EXPECT_EQ(error.Code, EErrorCode::JobFailed);
    EXPECT_EQ(error.Message, "File not found");
}

TEST(ErrorTest, StoresExceptionAsCode)
{
    Error error{EErrorCode::Exception, "File not found"};
    EXPECT_EQ(error.Code, EErrorCode::Exception);
}

TEST(ErrorTest, EmptyErrorDoesNotExist)
{
    static_assert(!std::is_default_constructible_v<Error>,
                  "Error must always be created with a code and a message");
}

TEST(ErrorTest, CopyError)
{
    Error error{EErrorCode::Exception, "Incorrect file name"};
    Error error1 = error;

    EXPECT_EQ(error1.Code, EErrorCode::Exception);
    EXPECT_EQ(error1.Message, "Incorrect file name");
}
