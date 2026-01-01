#include "EnginePch.h"
#include "TimeManager.h"

void TimeManager::Init()
{
    m_lastTime = glfwGetTime();
    m_currentTime = m_lastTime;
    m_deltaTime = 0.0f;
}

void TimeManager::Update()
{
    m_currentTime = glfwGetTime();
    m_deltaTime = static_cast<float>(m_currentTime - m_lastTime);

    // [Spiral of Death 방지]
    if (m_deltaTime > 0.25f) m_deltaTime = 0.25f;

    m_lastTime = m_currentTime;
    m_accumulator += m_deltaTime;
    CalculateFPS();
}

void TimeManager::CalculateFPS()
{
    m_frameCount++;
    if (m_currentTime - m_fpsLastTime >= 1.0)
    {
        m_fps = static_cast<float>(m_frameCount) / static_cast<float>(m_currentTime - m_fpsLastTime);
        m_frameCount = 0;
        m_fpsLastTime = m_currentTime;
    }
}

bool TimeManager::CheckFixedUpdate()
{
    // 누적된 시간이 고정 시간(1/60초)보다 크다면
    if (m_accumulator >= m_fixedDeltaTime)
    {
        // 고정 시간만큼 덜어내고 true 반환
        m_accumulator -= m_fixedDeltaTime;
        return true;
    }
    return false;
}