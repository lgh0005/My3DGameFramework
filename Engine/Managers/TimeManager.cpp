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

    // [중요] 나선형 죽음(Spiral of Death) 방지
    // 게임이 너무 느려져서 한 프레임에 1초가 걸렸다고 칩시다.
    // 이걸 그대로 물리 엔진에 넣으면 물리 업데이트를 60번 돌려야 하고,
    // 그러면 다음 프레임은 더 느려져서 영원히 멈춥니다.
    // 그래서 DeltaTime에 상한선(0.25초)을 둡니다.
    if (m_deltaTime > 0.25f) m_deltaTime = 0.25f;

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