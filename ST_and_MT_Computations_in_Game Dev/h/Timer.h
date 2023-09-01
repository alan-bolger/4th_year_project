// --------------------------------------------
// Timer.h
// --------------------------------------------
// This is a class that can be used to measure
// time intervals between code executions.
// --------------------------------------------

#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <string>
#include <iostream>

class Timer
{
public:
	Timer(std::string label) : label(label)
	{
		startTimepoint = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		
	}

	double stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();
		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimepoint).time_since_epoch();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch();
		auto duration = (end - start).count();
		double ms = duration * 0.001;

		return ms;
	}

private:
	std::string label;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTimepoint;
};

#endif // !TIMER_H