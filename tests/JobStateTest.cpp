/*
 Test written for checking the JobState behaviours and implementation is working correct or not
*/

#include <gtest/gtest.h>
#include <JobScheduler/Job.h>
#include <JobScheduler/EJobState.h>

// I had add CMakeLists.txt for this commit. Only task is that complete this test, add this file and commit
// both CMakeLists.txt and this test file will commits. Temporary comment

[[nodiscard]] bool IsValidTransition(EJobState From, EJobState To)
{
 return (From == EJobState::Created && To == EJobState::Pending) ||
        (From == EJobState::Pending && To == EJobState::Running) ||
        (From == EJobState::Pending && To == EJobState::Cancelled) ||
        (From == EJobState::Running && To == EJobState::Failed) ||
        (From == EJobState::Running && To == EJobState::Succeeded) ||
        (From == EJobState::Running && To == EJobState::Cancelled);
}

TEST(JobStateTest, IsDefaultJobStateIsCreated)
{
 std::function<void()> callable = [] () -> void {return;};
 JobId id(1);
 Job job(id, "PrintWork", EPriority::Normal, callable);
 EJobState CurrentState = job.GetState();
 EXPECT_EQ(CurrentState, EJobState::Created);
}

TEST(JobStateTest, IsValidTransition)
{
 std::function<void()> callable = [] () -> void {return;};
 JobId id(1);
 Job job(id, "PrintWork", EPriority::Normal, callable);

 EXPECT_TRUE(IsValidTransition(EJobState::Created, EJobState::Pending));
 EXPECT_TRUE(IsValidTransition(EJobState::Pending, EJobState::Running));
 EXPECT_TRUE(IsValidTransition(EJobState::Pending, EJobState::Cancelled));
 EXPECT_TRUE(IsValidTransition(EJobState::Running, EJobState::Succeeded));
 EXPECT_TRUE(IsValidTransition(EJobState::Running, EJobState::Failed));
 EXPECT_TRUE(IsValidTransition(EJobState::Running, EJobState::Cancelled));
}
