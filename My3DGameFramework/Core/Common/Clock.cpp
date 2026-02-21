#include "CorePch.h"
#include "Clock.h"

namespace MGF3D
{
    // 정적 멤버 초기화
    Clock::TimePoint Clock::s_baseTime;
    Clock::TimePoint Clock::s_prevTime;
    float  Clock::s_deltaTime = 0.0f;
    double Clock::s_totalTime = 0.0;
    float  Clock::s_timeScale = 1.0f;
    bool   Clock::s_isPaused = false;
    float  Clock::s_fixedDeltaTime = 1.0f / 60.0f;
    float  Clock::s_accumulator = 0.0f;
    float  Clock::s_fps = 0.0f;
    int32  Clock::s_frameCount = 0;
    float  Clock::s_fpsTimer = 0.0f;

    void Clock::Init()
    {
        s_baseTime = EngineClock::now();
        s_prevTime = s_baseTime;
        s_totalTime = 0.0;
        s_deltaTime = 0.0f;
        s_accumulator = 0.0f;
        s_fpsTimer = 0.0f;
        s_frameCount = 0;
    }

    void Clock::Update()
    {
        auto currentTime = EngineClock::now();
        std::chrono::duration<float> delta = currentTime - s_prevTime;
        s_deltaTime = delta.count();
        s_prevTime = currentTime;

        s_totalTime += static_cast<double>(s_deltaTime);

        if (!s_isPaused) s_accumulator += s_deltaTime * s_timeScale;

        s_frameCount++;
        s_fpsTimer += s_deltaTime;
        if (s_fpsTimer >= 1.0f)
        {
            s_fps = static_cast<float>(s_frameCount) / s_fpsTimer;
            s_frameCount = 0;
            s_fpsTimer = 0.0f;
        }
    }

    bool Clock::CheckFixedUpdate()
    {
        if (s_accumulator >= s_fixedDeltaTime)
        {
            s_accumulator -= s_fixedDeltaTime;
            return true;
        }
        return false;
    }

    double Clock::ToSeconds(uint64 microseconds)
    {
        return std::chrono::duration<double>(std::chrono::microseconds(microseconds)).count();
    }

    uint64 Clock::ToMicroseconds(double seconds)
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<double>(seconds)).count();
    }
}