#include "thread_pool.h"

ThreadPool::ThreadPool(): mThreadCount_(std::thread::hardware_concurrency() 
	!= 0 ? std::thread::hardware_concurrency() : 4), 
	mThreadQueues_(mThreadCount_) {}

ThreadPool::~ThreadPool() {}

void ThreadPool::Start()
{
for (auto i = 0; i < mThreadCount_; ++i) 
		mThreads_.emplace_back(&ThreadPool::Idle, this, i);
}

void ThreadPool::Stop()
{
	for (auto i = 0; i < mThreadCount_; ++i)
	{
	task_type emptyTask;
		mThreadQueues_[i].Push(emptyTask);
	}
	for (auto &th : mThreads_)
		if (th.joinable())
			th.join();
}

void ThreadPool::PushTask(func_type f, std::shared_ptr<int[]> arr, long l, 
	long r)
{
auto idQueueToPush = mIndex_++ % mThreadCount_;
task_type task = [=] {f(arr, l, r); };
mThreadQueues_[idQueueToPush].Push(task);
}

void ThreadPool::Idle(long qIndex)
{
	while (true) 
	{
		task_type task_to_do;
		bool isGotTask = false;
		auto i = 0;
		for (; i < mThreadCount_; ++i)
			if (isGotTask = mThreadQueues_[(qIndex + i) % 
				mThreadCount_].FastPop(task_to_do))
				break;
		if (!isGotTask)
			mThreadQueues_[qIndex].Pop(task_to_do);
		else if (!task_to_do)
			mThreadQueues_[(qIndex + i) % mThreadCount_].Push(task_to_do);
		if (!task_to_do) 
			return;
		task_to_do();
	}
}
