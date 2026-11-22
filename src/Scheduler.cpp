#include <JobScheduler/Scheduler.h>
#include <JobScheduler/JobQueue.h>
#include <cassert>

Result<JobHandle> Scheduler::Submit(SubmitSpecs specs)
{
    if (!specs.Callable) return Error{EErrorCode::InvalidArgument, "Callable cannot be empty"};
    JobId id(JobIdCounter++);
    Job job(id, std::move(specs.Name), specs.Priority, std::move(specs.Callable));

    JobHandle handle(job.GetId(), *this);
    records.emplace(id, Record());
    jobQueue.Push(std::move(job));

    return handle;
}

Result<Success> Scheduler::RunJob(Job& job)
{
    try
    {
        return job.Invoke();
    } catch (const std::exception& e)
    {
        return Error(EErrorCode::Exception, e.what());
    }
    catch (...)
    {
        return Error(EErrorCode::Exception, "Unknown exception");
    }
}

bool Scheduler::RunNext()
{
    auto job = jobQueue.Pop();
    if (!job) return false;

    [[maybe_unused]] const bool IsMoved = job->TryTransition(EJobState::Running);
    assert(IsMoved);
    auto it = records.find(job->GetId());
    assert(it != records.end());

    Record& record = it->second;
    record.State = EJobState::Running;
    const Result<Success> result = RunJob(*job);
    job->Complete(result);
    record.State = job->GetState();
    record.Outcome = job->GetOutcome();
    return true;
}
