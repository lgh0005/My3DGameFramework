#pragma once

namespace MGF3D
{
	// TODO : Core에 있는 Clock을 적극 이용해서 단순화
	// 디버그 빌드와 릴리즈 빌드를 서로 격리해야 함

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