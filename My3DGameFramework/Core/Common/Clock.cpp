#include "CorePch.h"
#include "Clock.h"

namespace MGF3D
{
	Clock::Clock() = default;
	Clock::~Clock() = default;

    void Clock::Init()
    {
        m_baseTime = EngineClock::now();
        m_prevTime = m_baseTime;
        m_totalTime = 0.0;
        m_deltaTime = 0.0f;
        m_accumulator = 0.0f;
        m_fpsTimer = 0.0f;
        m_frameCount = 0;
    }

    void Clock::Update()
    {
        auto currentTime = EngineClock::now();
        std::chrono::duration<float> delta = currentTime - m_prevTime;
        m_deltaTime = delta.count();
        m_prevTime = currentTime;

        // 전체 시간 누적
        m_totalTime += static_cast<double>(m_deltaTime);

        // FixedUpdate용 시간 누적 (일시정지 중에는 물리도 정지)
        if (!m_isPaused) m_accumulator += m_deltaTime * m_timeScale;

        // FPS 계산 (1초마다 갱신)
        m_frameCount++;
        m_fpsTimer += m_deltaTime;
        if (m_fpsTimer >= 1.0f)
        {
            m_fps = static_cast<float>(m_frameCount) / m_fpsTimer;
            m_frameCount = 0;
            m_fpsTimer = 0.0f;
        }
    }

    bool Clock::CheckFixedUpdate()
    {
        if (m_accumulator >= m_fixedDeltaTime)
        {
            m_accumulator -= m_fixedDeltaTime;
            return true;
        }
        return false;
    }
}