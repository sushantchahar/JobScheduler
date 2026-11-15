/*
 Test for checking the implementation and features of Job model
*/

#include <gtest/gtest.h>
#include <JobScheduler/EPriority.h>
#include <JobScheduler/JobId.h>
#include <JobScheduler/Job.h>
#include <JobScheduler/Result.h>
#include <JobScheduler/Success.h>
#include <string>
#include <functional>
#include <stdexcept>
#include <type_traits>

TEST(JobTest, StoresIdNameAndPriority)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<Result<Success>()> callable = []()
 {
  Success succ;
  Result<Success> result(succ);
  return result;
 };
 Job job(jobid, JobName, Priority, callable);

 EXPECT_EQ(job.GetId(), jobid);
 EXPECT_EQ(job.GetName(), JobName);
 EXPECT_EQ(job.GetPriority(), Priority);
}

TEST(JobTest, StartsInCreatedState)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<Result<Success>()> callable = []()
 {
  Success succ;
  Result<Success> result(succ);
  return result;
 };
 Job job(jobid, JobName, Priority, callable);

 EXPECT_EQ(job.GetState(), EJobState::Created);
}

TEST(JobTest, EmptyCallableThrowsInvalidArgument)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<Result<Success>()> callable; // empty callable
 EXPECT_THROW(Job(jobid, JobName, Priority, callable), std::invalid_argument);
}

TEST(JobTest, ValidTransitionChangesState)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<Result<Success>()> callable = []()
 {
  Success succ;
  Result<Success> result(succ);
  return result;
 };
 Job job(jobid, JobName, Priority, callable);

 bool result = job.TryTransition(EJobState::Pending);
 EXPECT_TRUE(result);
 EXPECT_EQ(job.GetState(), EJobState::Pending);
}

TEST(JobTest, InvalidTransitionRejected)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<Result<Success>()> callable = []()
 {
  Success succ;
  Result<Success> result(succ);
  return result;
 };
 Job job(jobid, JobName, Priority, callable);

 EXPECT_FALSE(job.TryTransition(EJobState::Running));
}

TEST(JobTest, ValidTransitionChainWorks)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<Result<Success>()> callable = []()
 {
  Success succ;
  Result<Success> result(succ);
  return result;
 };
 Job job(jobid, JobName, Priority, callable);

 EXPECT_TRUE(job.TryTransition(EJobState::Pending));
 EXPECT_TRUE(job.TryTransition(EJobState::Running));
 EXPECT_TRUE(job.TryTransition(EJobState::Succeeded));
}

TEST(JobTest, EndStateCannotMove)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<Result<Success>()> callable = []()
 {
  Success succ;
  Result<Success> result(succ);
  return result;
 };
 Job job(jobid, JobName, Priority, callable);

 EXPECT_TRUE(job.TryTransition(EJobState::Pending));
 EXPECT_TRUE(job.TryTransition(EJobState::Running));
 EXPECT_TRUE(job.TryTransition(EJobState::Succeeded));
 EXPECT_FALSE(job.TryTransition(EJobState::Running));
}

TEST(JobTest, InvokeReturnsSuccessResult)
{
 bool flag = false;
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<Result<Success>()> callable = [&flag] ()
 {
  Success succ;
  Result<Success> result(succ);
  flag = true;
  return result;
 };
 Job job(jobid, JobName, Priority, callable);
 EXPECT_TRUE(job.TryTransition(EJobState::Pending));
 EXPECT_TRUE(job.TryTransition(EJobState::Running));
 // before calling invoke flag == false?
 EXPECT_FALSE(flag);
 auto result = job.Invoke();
 EXPECT_TRUE(flag);
 EXPECT_TRUE(result.IsOk());
}

TEST(JobTest, InvokePassesThroughErrorUnchanged)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<Result<Success>()> callable = [] ()
 {
  Error error{EErrorCode::JobFailed, "File not found"};
  Result<Success> result(error);
  return result;
 };
 Job job(jobid, JobName, Priority, callable);
 EXPECT_TRUE(job.TryTransition(EJobState::Pending));
 EXPECT_TRUE(job.TryTransition(EJobState::Running));
 auto result = job.Invoke();
 EXPECT_FALSE(result.IsOk());
 EXPECT_EQ(result.GetError().Code, EErrorCode::JobFailed);
 EXPECT_EQ(result.GetError().Message, "File not found");
}

#ifndef NDEBUG
TEST(JobTest, InvokeOnNonRunningJobAsserts)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<Result<Success>()> callable = [] ()
 {
  Error error{EErrorCode::JobFailed, "File not found"};
  Result<Success> result(error);
  return result;
 };
 Job job(jobid, JobName, Priority, callable);
 EXPECT_DEBUG_DEATH((void)job.Invoke(), "");
}
#endif

TEST(JobTest, CompleteWithSuccessMovedToSucceeded)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<Result<Success>()> callable = [] ()
 {
  Success succ;
  Result<Success> result(succ);
  return result;
 };
 Job job(jobid, JobName, Priority, callable);
 EXPECT_TRUE(job.TryTransition(EJobState::Pending));
 EXPECT_TRUE(job.TryTransition(EJobState::Running));
 auto result = job.Invoke();
 job.Complete(result);
 EXPECT_EQ(job.GetState(), EJobState::Succeeded);
}

TEST(JobTest, CompleteWithErrorMovesToFailed)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<Result<Success>()> callable = [] ()
 {
  Error error{EErrorCode::JobFailed, "File not found"};
  Result<Success> result(error);
  return result;
 };
 Job job(jobid, JobName, Priority, callable);
 EXPECT_TRUE(job.TryTransition(EJobState::Pending));
 EXPECT_TRUE(job.TryTransition(EJobState::Running));
 auto result = job.Invoke();
 job.Complete(result);
 EXPECT_EQ(job.GetState(), EJobState::Failed);
}

#ifndef NDEBUG
TEST(JobTest, CompleteOnNonRunningJobAsserts)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<Result<Success>()> callable = [] ()
 {
  Error error{EErrorCode::JobFailed, "File not found"};
  Result<Success> result(error);
  return result;
 };
 Job job(jobid, JobName, Priority, callable);
 EXPECT_DEBUG_DEATH(job.Complete(Result<Success>(Success{})), "");
}
#endif

TEST(JobTest, GetOutcomeReturnsStoredSuccess)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<Result<Success>()> callable = [] ()
 {
  Success succ;
  Result<Success> result(succ);
  return result;
 };
 Job job(jobid, JobName, Priority, callable);
 EXPECT_TRUE(job.TryTransition(EJobState::Pending));
 EXPECT_TRUE(job.TryTransition(EJobState::Running));
 auto result = job.Invoke();
 job.Complete(result);
 const Result<Success>& outcome = job.GetOutcome();
 EXPECT_TRUE(outcome.IsOk());
}

TEST(JobTest, GetOutcomeReturnsStoredError)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<Result<Success>()> callable = [] ()
 {
  Error error{EErrorCode::JobFailed, "File not found"};
  Result<Success> result(error);
  return result;
 };
 Job job(jobid, JobName, Priority, callable);
 EXPECT_TRUE(job.TryTransition(EJobState::Pending));
 EXPECT_TRUE(job.TryTransition(EJobState::Running));
 job.Complete(job.Invoke());
 const Result<Success>& outcome = job.GetOutcome();
 EXPECT_FALSE(outcome.IsOk());
 EXPECT_TRUE(job.GetState() == EJobState::Failed);
}

#ifndef NDEBUG
TEST(JobTest, GetOutcomeBeforeCompleteAsserts)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<Result<Success>()> callable = [] ()
 {
  Error error{EErrorCode::JobFailed, "File not found"};
  Result<Success> result(error);
  return result;
 };
 Job job(jobid, JobName, Priority, callable);
 EXPECT_DEBUG_DEATH((void)job.GetOutcome(), "");
}
#endif

