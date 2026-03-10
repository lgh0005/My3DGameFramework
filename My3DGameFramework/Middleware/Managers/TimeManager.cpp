#include "CorePch.h"
#include "TimeManager.h"
#include "GLFWManager.h"

namespace MGF3D
{
    TimeManager::TimeManager() = default;
    TimeManager::~TimeManager() = default;

    bool TimeManager::Init()
    {
        // 1. GLFW로부터 현재 시간 얻어오기
        m_lastTime = MGF_GLFW.GetTime();

        // 2. GLFW 초기화 실패 혹은 내부 타이머 오류 시 0.0을 반환합니다.
        // 이 경우 false를 반환하여 Bootstrapper가 엔진을 안전하게 종료하도록 유도합니다.
        if (m_lastTime == 0.0)
        {
            MGF_LOG_ERROR("TimeManager: Failed to retrieve valid time. Is GLFW initialized?");
            return false;
        }

        // 3. 기본 델타타임 변수 초기화
        m_currentTime = m_lastTime;
        m_deltaTime = 0.0f;

        // 4. FPS 계산 및 FixedUpdate 누적기 초기화
        m_fpsLastTime = m_lastTime;
        m_frameCount = 0;
        m_fps = 0.0f;
        m_accumulator = 0.0;

        MGF_LOG_INFO("TimeManager: Successfully initialized.");
        return true;
    }

    void TimeManager::Shutdown()
    {
        m_lastTime = 0.0;
        m_currentTime = 0.0;
    }

    void TimeManager::Update()
    {
        m_currentTime = MGF_GLFW.GetTime();
        
        // 1. 이번 프레임에 걸린 실제 시간 (Real Frame Time)
        float frameTime = static_cast<float>(m_currentTime - m_lastTime);

        // 2. 명시적으로 선언된 한계치(MaxDeltaTime)로 클램핑 (Spiral of Death 방지)
        if (frameTime > m_maxDeltaTime)
            frameTime = m_maxDeltaTime;

        // 3. 엔진 전체에 뿌려질 안전한 델타 타임
        m_deltaTime = frameTime;

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
}