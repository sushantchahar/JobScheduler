#include <JobScheduler/WorkerPool.h>
#include <JobScheduler/JobExecution.h>

WorkerPool::WorkerPool(JobQueue& queue, size_t WorkersCount, StartCallback onStart, FinishCallback onFinish) :
jobQueue(queue),
OnStart(onStart),
OnFinish(onFinish)
{
    workers.reserve(WorkersCount);
    for (size_t i = 0; i < WorkersCount; ++i)
    {
        workers.emplace_back([this](std::stop_token token)
        {
            WorkerLoop(token);
        });
    }
}

void WorkerPool::WorkerLoop(std::stop_token token)
{
    while (true)
    {
        std::optional<Job> next = jobQueue.Pop(token);
        if (!next.has_value()) return;

        Job job = std::move(*next);
        const JobId id = job.GetId();

        assert(job.TryTransition(EJobState::Running));
        OnStart(id);
        Result<Success> result = JobExecuter::RunJobSafely(job);
        job.Complete(result);
        OnFinish(id, job.GetState(), std::move(result));
    }
}
