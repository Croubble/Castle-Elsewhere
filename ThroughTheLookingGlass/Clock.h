#pragma once
#include <Windows.h>
#include <iostream>
#include "Memory.h"
struct Clock
{
	LARGE_INTEGER clockFrequency;
	LARGE_INTEGER pos;
	LARGE_INTEGER end;
};

inline Clock* clock_create(Memory* memory)
{
	Clock* clock = (Clock*) memory_alloc(memory, sizeof(Clock));
	return clock;
}
inline void clock_start(Clock* clock)
{
	QueryPerformanceFrequency(&(clock->clockFrequency));
	QueryPerformanceCounter(&(clock->pos));
}

inline void clock_end(Clock* clock)
{
	QueryPerformanceCounter(&(clock->end));
	long int timeConsumed = clock->end.QuadPart - clock->pos.QuadPart;
	double time_in_millseconds = ((float) (1000 * timeConsumed) / (float) clock->clockFrequency.QuadPart);
	std::cout << "Time consumed in milliseconds" << time_in_millseconds << std::endl;
}