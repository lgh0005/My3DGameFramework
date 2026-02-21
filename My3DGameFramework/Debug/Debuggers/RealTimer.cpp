#include "DebugPch.h"
#include "RealTimer.h"

namespace MGF3D
{
    Clock::TimePoint RealTimer::s_startTime;

    void RealTimer::_Internal_Init()
    {
        s_startTime = Clock::EngineClock::now();
    }

    uint64 RealTimer::_Internal_GetTimestamp()
    {
        auto now = Clock::EngineClock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    }

    double RealTimer::_Internal_GetElapsedSeconds(uint64 start, uint64 end)
    {
        return Clock::ToSeconds(end - start);
    }

    double RealTimer::_Internal_GetTotalTime()
    {
        auto now = Clock::EngineClock::now();
        std::chrono::duration<double> elapsed = now - s_startTime;
        return elapsed.count();
    }
}