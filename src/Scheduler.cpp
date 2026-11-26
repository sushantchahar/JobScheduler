#include <JobScheduler/Scheduler.h>
#include <JobScheduler/JobQueue.h>
#include <JobScheduler/JobExecution.h>
#include <JobScheduler/WorkerPool.h>
#include <cassert>

Scheduler::Scheduler(std::size_t workerscount) :
workerPool(
    jobQueue,
    workerscount,
    [this](JobId id) {OnJobStarted(id);},
    [this](JobId id, EJobState state, Result<Success> result) {OnJobFinished(id, state, std::move(result));}
    )
{}

Result<JobHandle> Scheduler::Submit(SubmitSpecs specs)
{
    if (!specs.Callable) return Error{EErrorCode::InvalidArgument, "Callable cannot be empty"};
    JobId id(0);
    {
        std::lock_guard<std::mutex> lock(SchedulerMutex);
        id = JobId(JobIdCounter++);
        records.emplace(id, Record());
    }

    Job job(id, std::move(specs.Name), specs.Priority, std::move(specs.Callable));
    JobHandle handle(job.GetId(), *this);
    jobQueue.Push(std::move(job));

    return handle;
}

Scheduler::Record Scheduler::FindRecord(JobId id) const
{
    std::lock_guard<std::mutex> lock(SchedulerMutex);
    auto it = records.find(id);
    assert(it != records.end());

    return it->second;
}

void Scheduler::OnJobStarted(JobId id)
{
    std::lock_guard<std::mutex> lock(SchedulerMutex);
    auto it = records.find(id);
    assert(it != records.end());
    assert(it->second.State == EJobState::Pending);
    it->second.State = EJobState::Running;
}

void Scheduler::OnJobFinished(JobId id, EJobState state, Result<Success> result)
{
    std::lock_guard<std::mutex> lock(SchedulerMutex);
    auto it = records.find(id);
    assert(it != records.end());
    it->second.State = state;
    it->second.Outcome = std::move(result);
}

bool Scheduler::RunNext()
{
    assert(workerPool.WorkersCount() == 0);
    auto job = jobQueue.TryPop();
    if (!job) return false;

    [[maybe_unused]] const bool IsMoved = job->TryTransition(EJobState::Running);
    assert(IsMoved);
    OnJobStarted(job->GetId());

    const Result<Success> result = JobExecuter::RunJobSafely(*job);
    job->Complete(result);
    OnJobFinished(job->GetId(), job->GetState(), result);
    return true;
}

size_t Scheduler::RunAll()
{
    std::size_t ExecutedJobCounter = 0;
    while (RunNext())
    {
        ++ExecutedJobCounter;
    }

    return ExecutedJobCounter;
}
