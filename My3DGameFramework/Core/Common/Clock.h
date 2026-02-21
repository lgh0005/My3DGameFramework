#pragma once
#include <chrono>

namespace MGF3D
{
    class Clock
    {
        DECLARE_UTILITIES(Clock)

    public:
        using EngineClock = std::chrono::steady_clock;
        using TimePoint = std::chrono::time_point<EngineClock>;

    public:
        static void Init();
        static void Update();
        static bool CheckFixedUpdate();
        static double ToSeconds(uint64 microseconds);
        static uint64 ToMicroseconds(double seconds);

    public:
        static void  SetTimeScale(float scale) { s_timeScale = scale; }
        static void  SetPaused(bool paused) { s_isPaused = paused; }

        static float  GetDeltaTime() { return s_isPaused ? 0.0f : s_deltaTime * s_timeScale; }
        static float  GetUnscaledDeltaTime() { return s_deltaTime; }
        static double GetTotalTime() { return s_totalTime; }
        static float  GetFPS() { return s_fps; }
        static float  GetFixedDeltaTime() { return s_fixedDeltaTime; }

    private:
        static TimePoint s_baseTime;
        static TimePoint s_prevTime;

        static float  s_deltaTime;
        static double s_totalTime;
        static float  s_timeScale;
        static bool   s_isPaused;

        static float  s_fixedDeltaTime;
        static float  s_accumulator;

        static float  s_fps;
        static int32  s_frameCount;
        static float  s_fpsTimer;
    };
}