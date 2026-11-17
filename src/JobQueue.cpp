#include <JobScheduler/JobQueue.h>
#include <cassert>
#include <algorithm>

void JobQueue::Push(Job&& job)
{
    [[maybe_unused]] bool IsJobTransition = job.TryTransition(EJobState::Pending);
    assert(IsJobTransition);

    Entry entry(std::move(job), NextSequence++);
    Heap.push_back(std::move(entry));
    std::push_heap(Heap.begin(), Heap.end(), EntryCompare{});
}

std::optional<Job> JobQueue::Pop()
{
    if (Heap.empty()) return std::nullopt;
    std::pop_heap(Heap.begin(), Heap.end(), EntryCompare{});
    std::optional<Job> job = std::move(Heap.back().job);
    Heap.pop_back();
    return job;
}

std::size_t JobQueue::Size() const
{
    return Heap.size();
}

bool JobQueue::Empty() const
{
    return Heap.empty();
}
