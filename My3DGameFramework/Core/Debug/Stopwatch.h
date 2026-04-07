#pragma once
#include <chrono>

namespace MGF3D
{
	class Stopwatch
	{
		MGF_DECLARE_UTILITIES(Stopwatch)
		using Clock     = std::chrono::steady_clock;
		using TimePoint = std::chrono::time_point<Clock>;

	public:
		static void   _Internal_Init();
		static uint64 _Internal_GetTimestamp();
		static double _Internal_GetElapsedSeconds(uint64 start, uint64 end);
		static double _Internal_GetTotalTime();

	private:
		static TimePoint s_startTime;
	};
}
