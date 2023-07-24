#include "ThreadPool.h"

/// <summary>
/// ThreadPool constructor.
/// </summary>
/// <param name="threadAmount">Set the number of threads in the pool.</param>
ThreadPool::ThreadPool(std::size_t threadAmount)
{
	start(threadAmount);
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
/// <param name="threadAmount">Set the number of threads in the pool.</param>
void ThreadPool::start(std::size_t threadAmount)
{
	for (auto i = 0u; i < threadAmount; ++i)
	{
		threads.emplace_back([=]
		{
			// Loop runs as long as there's jobs
			while (true)
			{
				Job job;

				// New scope
				{
					std::unique_lock<std::mutex> lock{ eventMutex };
					eventVar.wait(lock, [=] { return stopping || !jobs.empty(); });

					// Break out of loop if thread is stopping and there's no jobs left
					if (stopping && jobs.empty())
					{
						break;
					}

					// Move front job to current job
					job = std::move(jobs.front());
					jobs.pop();
				}
				
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
	// New scope
	{
		std::unique_lock<std::mutex> lock{ eventMutex };
		stopping = true;
	}

	eventVar.notify_all();

	// Close threads
	for (auto &thread : threads)
	{
		thread.join();
	}
}