#include "DebugPch.h"
#include "RealTimer.h"

namespace MGF3D
{
	RealTimer::TimePoint RealTimer::m_startTime;

    void RealTimer::Init()
    {
        m_startTime = Clock::now();
    }

    uint64 RealTimer::GetTimestamp()
    {
        auto now = Clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    }

    double RealTimer::GetElapsedSeconds(uint64 start, uint64 end)
    {
        return static_cast<double>(end - start) / 1000000.0;
    }

    double RealTimer::GetTotalTime()
    {
        auto now = Clock::now();
        std::chrono::duration<double> elapsed = now - m_startTime;
        return elapsed.count();
    }
}