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
    m_lastTime = m_currentTime;
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
