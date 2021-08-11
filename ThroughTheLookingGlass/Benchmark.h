#pragma once
#include <windows.h>
#include <stdio.h>
#include <iostream>

//Library for very, very basic stopwatch timing. Lets be honest, if you aren't me, you shouldn't be using this library. I'm only using it because its educational. 
//	this is a single header library, so to include this in a project make sure to define benchmark implementation exactly once in a source file before including. i.e.
//		#define BENCHMARK_IMPLEMENTATION	
//		#include "Benchmark.h"
//	exactly once in a SOURCE file (not header file) before including
#ifdef BENCHMARK_IMPLEMENTATION
#define start_stopwatch() do{debug_start_stopwatch(__LINE__,__FILE__);} while(0)
#define end_stopwatch() do{debug_stop_stopwatch(__LINE__,__FILE__);}while(0)
struct Timer
{
	bool started = false;
	LARGE_INTEGER start_time;
	LARGE_INTEGER end_time;
	char start_file_name[512];
	int start_file_num;
};

Timer timer;
void debug_start_stopwatch(int line, const char* file)
{
	if (timer.started)
	{
		printf("tried to start timer that was already running on line %d in file %s\nmake sure you stop a timer before starting it",line,file);
		abort();
	}
	timer.started = true;
	timer.start_file_num = line;
	int i;
	for (i = 0; file[i] != '\0' && i < 512; i++)
	{
		timer.start_file_name[i] = file[i];
	}
	timer.start_file_name[i] = '\0';
	bool succeeded = QueryPerformanceCounter(&timer.start_time);
	if (!succeeded)
	{
		printf("timer failed to log at line %u in file %s", __LINE__, __FILE__);
		abort();
	}
}
void debug_stop_stopwatch(int line, const char* file)
{
	bool succeeded = QueryPerformanceCounter(&timer.end_time);
	if (!timer.started)
	{
		printf("tried to stop timer that wasn't running on line %d in file %s\nmake sure you start a timer before stopping it", line, file);
		abort();
	}
	timer.started = false;
	if (!succeeded)
	{
		printf("timer failed to log at line %u in file %s", __LINE__, __FILE__);
		abort();
	}

	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	long long int ticks = timer.end_time.QuadPart - timer.start_time.QuadPart;
	long long int num_ticks_each_second = frequency.QuadPart;
	long long int num_ticks_each_microsecond = num_ticks_each_second / 1000000;
	long long int num_ticks_each_millisecond = num_ticks_each_second / 1000;
	long long int total_time_in_microseconds = ticks / num_ticks_each_microsecond;
	double total_time_in_milliseconds = ((double)ticks) / ((double)num_ticks_each_millisecond);
	double total_time_in_seconds = ((double)ticks) / ((double) num_ticks_each_second);
	printf("clock report: \nstarted on line %u in file %s\nended on line %u in file %s\n total time %lld microseconds\ntotal time %f milliseconds\ntotal time %f seconds", 
		timer.start_file_num, timer.start_file_name, 
		line, file, 
		total_time_in_microseconds, 
		total_time_in_milliseconds,
		total_time_in_seconds);

}

#endif
