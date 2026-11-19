#include <JobScheduler/Scheduler.h>

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
