#include "CorePch.h"
#include "Stopwatch.h"

namespace MGF3D
{
	Stopwatch::TimePoint Stopwatch::s_startTime;

	void Stopwatch::_Internal_Init()
	{
		s_startTime = Clock::now();
	}

	uint64 Stopwatch::_Internal_GetTimestamp()
	{
		auto now = Clock::now();
		return static_cast<uint64>(std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count());
	}

	double Stopwatch::_Internal_GetElapsedSeconds(uint64 start, uint64 end)
	{
		return static_cast<double>(end - start) / 1000000.0;
	}

	double Stopwatch::_Internal_GetTotalTime()
	{
		auto now = Clock::now();
		std::chrono::duration<double> elapsed = now - s_startTime;
		return elapsed.count();
	}
}