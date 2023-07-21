#include "ThreadPool.h"

/// <summary>
/// ThreadPool constructor.
/// </summary>
/// <param name="numThreads">Set the number of threads to be used.</param>
ThreadPool::ThreadPool(std::size_t numThreads)
{
	start(numThreads);
}

/// <summary>
/// ThreadPool destructor.
/// </summary>
ThreadPool::~ThreadPool()
{
	stop();
}

/// <summary>
/// Start pool.
/// </summary>
/// <param name="numThreads">Set the number of threads to be used.</param>
void ThreadPool::start(std::size_t numThreads)
{
	for (unsigned int i = 0u; i < numThreads; ++i)
	{
		threads.emplace_back([=] 
		{
			while (true)
			{
				Job job;
				std::unique_lock<std::mutex> lock{ eventMutex };
				eventVar.wait(lock, [=] { return stopping || !jobs.empty(); });

				// Break out of loop if thread is stopping and there's no jobs left
				if (stopping && jobs.empty())
				{
					break;
				}

				job = std::move(jobs.front());
				jobs.pop();
				job();
			}
		});
	}
}

/// <summary>
/// Stop all threads.
/// </summary>
void ThreadPool::stop() noexcept
{
	std::unique_lock<std::mutex> lock{ eventMutex };
	stopping = true;
	eventVar.notify_all();

	for (std::thread &thread : threads)
	{
		thread.join();
	}
}
