// --------------------------------------------
// | ThreadPool.h                             |
// | ThreadPool.cpp							  |
// | -----------------------------------------|
// | This class creates a threadpool. By	  |
// | default, it uses the maximum amount	  |
// | of threads available. Use the			  |
// | constructor parameter to set the number  |
// | of threads manually.					  |
// --------------------------------------------

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <condition_variable>
#include <functional>
#include <iostream>
#include <future>
#include <vector>
#include <thread>
#include <queue>

class ThreadPool
{
public:
	using Job = std::function<void()>;

	explicit ThreadPool(std::size_t threadAmount = std::thread::hardware_concurrency());
	~ThreadPool();
	
	template<class T>
	auto addJob(T job)->std::future<decltype(job())>
	{
		auto wrapper = std::make_shared<std::packaged_task<decltype(job()) ()>>(std::move(job));

		{
			std::unique_lock<std::mutex> lock{ eventMutex };
			jobs.emplace([=] { (*wrapper)(); });
		}

		eventVar.notify_one();
		return wrapper->get_future();
	}

private:
	std::vector<std::thread> threads;
	std::condition_variable eventVar;
	std::mutex eventMutex;
	bool stopping = false;
	std::queue<Job> jobs;

	void start(std::size_t threadAmount);
	void stop() noexcept;
};

#endif // !THREADPOOL_H
