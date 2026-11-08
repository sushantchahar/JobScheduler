/*
 Test for checking the implementation and features of Job model
*/

#include <gtest/gtest.h>
#include <JobScheduler/EPriority.h>
#include <JobScheduler/JobId.h>
#include <JobScheduler/Job.h>
#include <string>

TEST(JobTest, StoresIdNameAndPriority)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<void()> callable = []() -> void {return;};
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
 std::function<void()> callable = []() -> void {return;};
 Job job(jobid, JobName, Priority, callable);

 EXPECT_EQ(job.GetState(), EJobState::Created);
}

TEST(JobTest, EmptyCallableThrowsInvalidArgument)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<void()> callable; // empty callable
 EXPECT_THROW(Job(jobid, JobName, Priority, callable), std::invalid_argument);
}

TEST(JobTest, ValidTransitionChangesState)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<void()> callable = []() -> void {return;};
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
 std::function<void()> callable = []() -> void {return;};
 Job job(jobid, JobName, Priority, callable);

 EXPECT_FALSE(job.TryTransition(EJobState::Running));
}

TEST(JobTest, ValidTransitionChainWorks)
{
 std::string JobName = "PrintReport";
 JobId jobid(1);
 EPriority Priority = EPriority::High;
 std::function<void()> callable = []() -> void {return;};
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
 std::function<void()> callable = []() -> void {return;};
 Job job(jobid, JobName, Priority, callable);

 EXPECT_TRUE(job.TryTransition(EJobState::Pending));
 EXPECT_TRUE(job.TryTransition(EJobState::Running));
 EXPECT_TRUE(job.TryTransition(EJobState::Succeeded));
 EXPECT_FALSE(job.TryTransition(EJobState::Running));
}