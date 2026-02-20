#pragma once
#include <chrono>

namespace MGF3D
{
    class Clock
    {
    public:
        Clock();
        ~Clock();
        using EngineClock = std::chrono::steady_clock;
        using TimePoint = std::chrono::time_point<EngineClock>;

    public:
        void Init();
        void Update();
        bool CheckFixedUpdate();

    public:
        void SetTimeScale(float scale) { m_timeScale = scale; }
        void SetPaused(bool paused) { m_isPaused = paused; }

        float  GetDeltaTime() const { return m_isPaused ? 0.0f : m_deltaTime * m_timeScale; }
        float  GetUnscaledDeltaTime() const { return m_deltaTime; }
        double GetTotalTime() const { return m_totalTime; }
        float  GetFPS() const { return m_fps; }
        float  GetFixedDeltaTime() const { return m_fixedDeltaTime; }

    private:
        TimePoint m_baseTime;      // 초기화 시점
        TimePoint m_prevTime;      // 이전 프레임 시점

        float  m_deltaTime = 0.0f;
        double m_totalTime = 0.0;
        float  m_timeScale = 1.0f;
        bool   m_isPaused = false;

        float  m_fixedDeltaTime = 1.0f / 60.0f; // 기본 60Hz
        float  m_accumulator = 0.0f;

        float  m_fps = 0.0f;
        int32  m_frameCount = 0;
        float  m_fpsTimer = 0.0f;
    };
}