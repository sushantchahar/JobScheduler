#include <gtest/gtest.h>
#include <JobScheduler/Result.h>
#include <JobScheduler/Error.h>
#include <JobScheduler/Success.h>
#include <type_traits>

TEST(ResultTest, ResultSuccessCreatedFromSuccess)
{
    Success success;
    Result<Success> result(success);
    EXPECT_TRUE(result.IsOk());
}

TEST(ResultTest, ResultSuccessCreatedFromError)
{
    Error error{EErrorCode::JobFailed, "file not found"};
    Result<Success> result(error);
    EXPECT_FALSE(result.IsOk());
}

TEST(ResultTest, GetErrorProvideSameErrorCodeAndMessage)
{
    Error error{EErrorCode::Exception, "file not found"};
    Result<Success> result(error);
    const Error& er = result.GetError();
    EXPECT_TRUE(er.Code == EErrorCode::Exception);
    EXPECT_TRUE(er.Message == "file not found");
}

TEST(ResultTest, GetValueUsingErrorsGetValueMethod)
{
    Result<float> result(45.f);
    auto value = result.GetValue();
    EXPECT_TRUE(value == 45.f);
}

TEST(ResultTest, CopyResultAlsoCopySameTypeAndContent)
{
    Result<float> result1(56.f);
    Result<float> result2(result1);
    Result<Success> result3(Error{EErrorCode::JobFailed, "file not found"});
    Result<Success> result4(result3);
    EXPECT_TRUE(result1.IsOk());
    EXPECT_TRUE(result2.IsOk());
    EXPECT_EQ(result1.GetValue(), result2.GetValue());
    EXPECT_FALSE(result3.IsOk());
    EXPECT_FALSE(result4.IsOk());
    EXPECT_EQ(result3.GetError().Code, result4.GetError().Code);
    EXPECT_EQ(result3.GetError().Message, result4.GetError().Message);
}

TEST(ResultTest, SuccessResultIsNotDefaultConstructible)
{
    static_assert(!std::is_default_constructible_v<Result<Success>>);
}

#ifndef NDEBUG
TEST(ResultTest, CallingGetValueOnErrorAndGetErrorOnSuccess)
{
    Result<Success> SuccessResult{Success{}};
    Result<Success> ErrorResult{Error{EErrorCode::JobFailed, "file not found"}};

    EXPECT_DEBUG_DEATH(SuccessResult.GetError(), "");
    EXPECT_DEBUG_DEATH(ErrorResult.GetValue(), "");
}
#endif