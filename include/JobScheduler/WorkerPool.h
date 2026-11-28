#ifndef JOBSCHEDULER_WORKERPOOL_H
#define JOBSCHEDULER_WORKERPOOL_H

#include <JobScheduler/JobQueue.h>
#include <JobScheduler/JobId.h>
#include <JobScheduler/Result.h>
#include <JobScheduler/Success.h>
#include <functional>
#include <thread>
#include <vector>
#include <stop_token>

class WorkerPool
{
    using StartCallback = std::function<void(JobId id)>;
    using FinishCallback = std::function<void(JobId, EJobState, Result<Success>)>;

private:
    void WorkerLoop(std::stop_token token);

    JobQueue& jobQueue;
    StartCallback OnStart;
    FinishCallback OnFinish;
    std::vector<std::jthread> workers;

public:
    WorkerPool(JobQueue& queue, size_t WorkersCount, StartCallback onStart, FinishCallback onFinish);
    WorkerPool(const WorkerPool&) = delete;
    WorkerPool& operator=(const WorkerPool&) = delete;
    [[nodiscard]] std::size_t WorkersCount() const {return workers.size();}
    void Stop();
    ~WorkerPool();
};

#endif //JOBSCHEDULER_WORKERPOOL_H
