#include <JobScheduler/JobQueue.h>
#include <cassert>
#include <algorithm>

void JobQueue::Push(Job&& job)
{
    std::lock_guard<std::mutex> lock(QueueMutex);
    [[maybe_unused]] bool IsJobTransition = job.TryTransition(EJobState::Pending);
    assert(IsJobTransition);

    Entry entry(std::move(job), NextSequence++);
    Heap.push_back(std::move(entry));
    std::push_heap(Heap.begin(), Heap.end(), EntryCompare{});
    QueueCondition.notify_one();
}

std::optional<Job> JobQueue::TryPop()
{
    std::lock_guard<std::mutex> lock(QueueMutex);
    return PopLocked();
}

std::optional<Job> JobQueue::PopLocked()
{
    if (Heap.empty()) return std::nullopt;
    std::pop_heap(Heap.begin(), Heap.end(), EntryCompare{});
    std::optional<Job> job = std::move(Heap.back().job);
    Heap.pop_back();
    return job;
}

std::optional<Job> JobQueue::Pop()
{
    // It blocks pop when queue is empty
    std::unique_lock<std::mutex> lock(QueueMutex);
    QueueCondition.wait(lock, [this]() { return !Heap.empty(); });
    return PopLocked();
}

std::size_t JobQueue::Size() const
{
    std::lock_guard<std::mutex> lock(QueueMutex);
    return Heap.size();
}

bool JobQueue::Empty() const
{
    std::lock_guard<std::mutex> lock(QueueMutex);
    return Heap.empty();
}
