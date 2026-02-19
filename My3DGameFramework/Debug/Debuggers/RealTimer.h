#pragma once

namespace MGF3D
{
	class RealTimer
	{
		DECLARE_UTILITIES(RealTimer)

	public:
		static void Init();
		static uint64 GetTimestamp();
		static double GetElapsedSeconds(uint64 start, uint64 end);
		static double GetTotalTime();

	private:
		using Clock = std::chrono::steady_clock;
		using TimePoint = std::chrono::time_point<Clock>;

		static TimePoint m_startTime;
	};
}