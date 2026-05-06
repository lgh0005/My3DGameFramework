#include "FrameworkPch.h"
#include "AnimController.h"
#include "Managers/TypeManager.h"
#include "Assets/Animation.h"
#include "Mechanics/Animation/AnimState.h"

namespace MGF3D
{
	AnimController::AnimController(Component* owner) : Super(owner) { }
	AnimController::~AnimController() = default;
	AnimController::AnimController(AnimController&& other) noexcept = default;
	AnimController& AnimController::operator=(AnimController&& other) noexcept = default;
	
	/*================================//
	//   MGF3D Mechanic Custom Type   //
	//================================*/
	int16 AnimController::s_typeIndex = -1;
	const MGFType* AnimController::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Mechanic");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void AnimController::AddState(const String& name, const AnimationPtr& clip, bool isLoop, float speed)
	{
		auto state = AnimStatePtr(new AnimState(name, clip));
		state->SetLoop(isLoop);
		state->SetSpeed(speed);
		m_states[state->GetNameHash()] = std::move(state);
	}

	void AnimController::SetTransitionDuration(StringHash fromHash, StringHash toHash, float duration)
	{
		auto it = m_states.find(fromHash);
		if (it != m_states.end()) it->second->AddTransition(toHash, duration);
	}

	void AnimController::SetStartState(StringHash stateHash)
	{
		auto it = m_states.find(stateHash);
		if (it != m_states.end())
		{
			m_currentState = it->second;
			m_currentTime = 0.0f;

			// 초기화 시 블렌딩 상태 정리
			m_nextState = nullptr;
			m_isBlending = false;
		}
	}

	void AnimController::Play(StringHash stateHash)
	{
		CrossFade(stateHash, 0.0f);
	}

	void AnimController::CrossFade(StringHash nextStateHash, float duration)
	{
		auto it = m_states.find(nextStateHash);
		if (it == m_states.end()) return; 

		auto targetState = it->second;

		// 이미 그 상태면 무시
		if (m_currentState == targetState) return;
		if (m_isBlending && m_nextState == targetState) return;

		// [Duration 결정 로직]
		if (duration < 0.0f && m_currentState)
		{
			duration = m_currentState->GetTransitionDuration(nextStateHash);
			if (duration < 0.0f) duration = 0.2f;
		}

		// 음수 방지
		if (duration < 0.0f) duration = 0.0f;

		TransitTo(targetState, duration);
	}

	void AnimController::TransitTo(const AnimStatePtr& nextState, float duration)
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
		if (m_currentState && m_currentState->GetAnimation())
		{
			auto clip = m_currentState->GetAnimation();
			m_currentTime += deltaTime * m_currentState->GetSpeed();

			float duration = clip->GetDuration();
			if (duration > 0.0f)
			{
				if (m_currentState->IsLoop()) m_currentTime = fmod(m_currentTime, duration);
				else m_currentTime = glm::clamp(m_currentTime, 0.0f, duration);
			}
		}

		// 2. Next State & Blending Update
		if (m_isBlending && m_nextState && m_nextState->GetAnimation())
		{
			auto anim = m_nextState->GetAnimation();
			m_nextTime += deltaTime * m_nextState->GetSpeed();

			float duration = anim->GetDuration();
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

	Pose AnimController::GetPose(StringHash nodeHash, const Pose& defaultPose) const
	{
		Pose poseA = defaultPose;

		// A. 현재 상태 포즈 추출
		if (m_currentState && m_currentState->GetAnimation())
		{
			auto anim = m_currentState->GetAnimation();
			const AnimChannel* ch = anim->FindChannel(nodeHash);

			if (ch)
			{
				float timeInTicks = m_currentTime * anim->GetTicksPerSecond();
				poseA = ch->GetPose(timeInTicks);
			}
		}

		// 블렌딩 중이 아니라면 바로 리턴
		if (!m_isBlending) return poseA;

		// B. 다음 상태 포즈 추출 (블렌딩)
		Pose poseB = defaultPose;
		if (m_nextState && m_nextState->GetAnimation())
		{
			auto animNext = m_nextState->GetAnimation();
			const AnimChannel* chNext = animNext->FindChannel(nodeHash);

			if (chNext)
			{
				float nextTimeInTicks = m_nextTime * animNext->GetTicksPerSecond();
				poseB = chNext->GetPose(nextTimeInTicks);
			}

			// 두 포즈를 구면 선형 보간(Slerp/Mix)하여 최종 결과 반환
			return Pose::Interpolate(poseA, poseB, m_finalBlendFactor);
		}

		return poseA;
	}

	mat4 AnimController::GetLocalMatrix(StringHash nodeHash, const mat4& defaultMatrix) const
	{
		// 이 함수는 블렌딩이 필요 없을 때 Pose 변환/보간 비용을 아끼기 위해 사용 가능합니다.
		if (m_isBlending) return GetPose(nodeHash, Pose::FromMat4(defaultMatrix)).ToMat4();

		if (m_currentState && m_currentState->GetAnimation())
		{
			auto anim = m_currentState->GetAnimation();
			const AnimChannel* ch = anim->FindChannel(nodeHash);
			if (ch)
			{
				float timeInTicks = m_currentTime * anim->GetTicksPerSecond();
				return ch->GetPose(timeInTicks).ToMat4();
			}
		}

		return defaultMatrix;
	}
}
