#include <thread>
#include <queue>
#include <vector>
#include <iostream>
#include <functional>
#include <condition_variable>

#include "dispatch_queue.h"

dispatch_queue::dispatch_queue(size_t thread_cnt) :
	threads_(thread_cnt)
{
        std::cout << "TID: " << std::this_thread::get_id() << "\tTHREADS: " << thread_cnt << "\tCONSTRUCTOR" << std::endl;
	for(size_t i = 0; i < threads_.size(); i++)
	{
		threads_[i] = std::thread(
			std::bind(&dispatch_queue::dispatch_thread_handler, this));
	}
}

dispatch_queue::~dispatch_queue()
{
        std::cout << "TID: " << std::this_thread::get_id() << "\tTHREADS: " << threads_.size() << "\tDESTRUCTOR" << std::endl;
	// Signal to dispatch threads that it's time to wrap up
	quit_ = true;
	cv_.notify_all();

	// Wait for threads to finish before we exit
	for(size_t i = 0; i < threads_.size(); i++)
	{
		if(threads_[i].joinable())
		{
			threads_[i].join();
		}
	}
}

void dispatch_queue::dispatch(const fp_t& op)
{
	std::unique_lock<std::mutex> lock(lock_);
	q_.push(op);

	// Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
	lock.unlock();
	cv_.notify_all();
}

void dispatch_queue::dispatch(fp_t&& op)
{
	std::unique_lock<std::mutex> lock(lock_);
	q_.push(std::move(op));

	// Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
	lock.unlock();
	cv_.notify_all();
}

void dispatch_queue::dispatch_thread_handler(void)
{
	std::unique_lock<std::mutex> lock(lock_);

	do {
		//Wait until we have data or a quit signal
		cv_.wait(lock, [this]{
			return (q_.size() || quit_);
		});

		//after wait, we own the lock
		if(q_.size() && !quit_)
		{
			auto op = std::move(q_.front());
			q_.pop();

			//unlock now that we're done messing with the queue
			lock.unlock();

			op();

			lock.lock();
		}
	} while (!quit_);
}
