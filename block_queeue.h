#pragma once
#include <mutex>
#include <queue>
#include <condition_variable>

template<class T> class BlockedQueue
{
  std::mutex mLocker_;
  
  std::queue<T> mTaskQueue_;
 
  std::condition_variable mNotifier_;
public:

void Push(T &item)
	{
		std::lock_guard<std::mutex> locker(mLocker_);
		mTaskQueue_.push(item);
	
		mNotifier_.notify_one();
	}

void Pop(T &item)
	{
		std::unique_lock<std::mutex> locker(mLocker_);
		
		if (mTaskQueue_.empty())
			mNotifier_.wait(locker, [this] {return !mTaskQueue_.empty(); });
		
		item = mTaskQueue_.front();
		mTaskQueue_.pop();
	}

	auto FastPop(T &item) -> bool
	{
		std::lock_guard<std::mutex> locker(mLocker_);
if (mTaskQueue_.empty()) 
			return false;
		
    item = mTaskQueue_.front();
		mTaskQueue_.pop();
		return true;
  }
};
