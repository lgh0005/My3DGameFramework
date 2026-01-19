#include "EnginePch.h"
#include "AnimController.h"
#include "Resources/Animations/Animation.h"
#include "Resources/Animations/AnimState.h"
#include "Resources/Animations/AnimChannel.h"
#include "Resources/Animations/Pose.h"

DECLARE_DEFAULTS_IMPL(AnimController)

AnimControllerUPtr AnimController::Create()
{
    return AnimControllerUPtr(new AnimController());
}

void AnimController::AddState(const std::string& name, AnimationPtr clip, bool isLoop, float speed)
{
    auto state = AnimState::Create(name, clip);
    state->SetLoop(isLoop);
    state->SetSpeed(speed);
    m_states[name] = std::move(state);
}

void AnimController::SetTransitionDuration(const std::string& from, const std::string& to, float duration)
{
    auto it = m_states.find(from);
    if (it != m_states.end()) it->second->AddTransition(to, duration);
}

void AnimController::SetStartState(const std::string& name)
{
    auto it = m_states.find(name);
    if (it != m_states.end())
    {
        m_currentState = it->second;
        m_currentTime = 0.0f;

        // 초기화 시 블렌딩 상태 정리
        m_nextState = nullptr;
        m_isBlending = false;
    }
}

void AnimController::Play(const std::string& stateName)
{
    CrossFade(stateName, 0.0f); // 0초 블렌딩 = 즉시 재생
}

void AnimController::CrossFade(const std::string& nextStateName, float duration)
{
    auto it = m_states.find(nextStateName);
    if (it == m_states.end()) return; // 없는 상태면 무시

    auto targetState = it->second;

    // 이미 그 상태면 무시
    if (m_currentState == targetState) return;
    if (m_isBlending && m_nextState == targetState) return;

    // [Duration 결정 로직]
    if (duration < 0.0f && m_currentState)
    {
        // AnimState 클래스의 캡슐화된 메서드 사용
        duration = m_currentState->GetTransitionDuration(nextStateName);
        if (duration < 0.0f) duration = 0.2f; // 기본값
    }

    // 음수 방지
    if (duration < 0.0f) duration = 0.0f;

    TransitTo(targetState, duration);
}

void AnimController::TransitTo(std::shared_ptr<AnimState> nextState, float duration)
{
    m_nextState = nextState;
    m_nextTime = 0.0f;
    m_blendDuration = duration;
    m_blendTimer = 0.0f;
    m_finalBlendFactor = 0.0f;
    m_isBlending = (duration > 0.0f);

    // 즉시 교체
    if (!m_isBlending)
    {
        m_currentState = m_nextState;
        m_currentTime = 0.0f;
        m_nextState = nullptr;
    }
}

void AnimController::Update(float deltaTime)
{
    // 1. Current State Update
    if (m_currentState && m_currentState->GetClip())
    {
        auto clip = m_currentState->GetClip();
        m_currentTime += clip->GetTicksPerSecond() * deltaTime * m_currentState->GetSpeed();

        float duration = clip->GetDuration();
        if (duration > 0.0f)
        {
            if (m_currentState->IsLoop()) m_currentTime = fmod(m_currentTime, duration);
            else m_currentTime = glm::clamp(m_currentTime, 0.0f, duration);
        }
    }

    // 2. Next State & Blending Update
    if (m_isBlending && m_nextState && m_nextState->GetClip())
    {
        auto clip = m_nextState->GetClip();
        m_nextTime += clip->GetTicksPerSecond() * deltaTime * m_nextState->GetSpeed();

        float duration = clip->GetDuration();
        if (duration > 0.0f)
        {
            if (m_nextState->IsLoop()) m_nextTime = fmod(m_nextTime, duration);
            else m_nextTime = glm::clamp(m_nextTime, 0.0f, duration);
        }

        m_blendTimer += deltaTime;

        if (m_blendDuration > 0.0f) m_finalBlendFactor = glm::clamp(m_blendTimer / m_blendDuration, 0.0f, 1.0f);
        else m_finalBlendFactor = 1.0f;

        if (m_blendTimer >= m_blendDuration)
        {
            m_currentState = m_nextState;
            m_currentTime = m_nextTime;
            m_nextState = nullptr;
            m_isBlending = false;
            m_blendTimer = 0.0f;
            m_finalBlendFactor = 0.0f;
        }
    }
}

Pose AnimController::GetPose(uint32 nodeNameHash, const Pose& defaultPose) const
{
    // A. 현재 포즈
    Pose poseA = defaultPose;
    bool hasA = false;

    // A. 현재 상태 포즈
    if (m_currentState && m_currentState->GetClip())
    {
        // Animation -> AnimChannel -> GetPose
        const AnimChannel* ch = m_currentState->GetClip()->FindChannel(nodeNameHash);
        if (ch)
        {
            poseA = ch->GetPose(m_currentTime);
            hasA = true;
        }
    }

    if (!m_isBlending) return poseA;

    // B. 다음 상태 포즈 (블렌딩)
    if (m_nextState && m_nextState->GetClip())
    {
        Pose poseB = defaultPose;
        bool hasB = false;

        const AnimChannel* chNext = m_nextState->GetClip()->FindChannel(nodeNameHash);
        if (chNext)
        {
            poseB = chNext->GetPose(m_nextTime);
            hasB = true;
        }

        // 둘 중 하나라도 애니메이션이 있으면 섞는다
        if (hasA || hasB)
            return Pose::Interpolate(poseA, poseB, m_finalBlendFactor);
    }

    return poseA;
}