#include "EnginePch.h"
#include "Velocitybuffer.h"

DECLARE_DEFAULTS_IMPL(Velocitybuffer)

VelocitybufferUPtr Velocitybuffer::Create()
{
	return VelocitybufferUPtr(new Velocitybuffer());
}

void Velocitybuffer::RecordMovement(const glm::vec3& displacement, float dt)
{
	// 1. 움직임을 기록합니다. (매 프레임/물리 틱마다 호출)
	m_accumulatedDisplacement += displacement;
	m_accumulatedTime += dt;

	// 2. 감쇠(Decay) 필터
	// 최대 윈도우(약 33ms = 60Hz 2프레임 분량)를 유지하도록 오래된 데이터를 밀어냅니다.
	if (m_accumulatedTime > m_maxWindow)
	{
		float decayFactor = m_maxWindow / m_accumulatedTime;
		m_accumulatedDisplacement *= decayFactor;
		m_accumulatedTime = m_maxWindow;
	}
}

glm::vec3 Velocitybuffer::GetSmoothVelocity(float targetDt) const
{
	// 데이터가 거의 없으면(0.0001초 미만) 움직임 없음으로 처리
	if (m_accumulatedTime < 1e-6f)
		return glm::vec3(0.0f);

	// 1. 현재 버퍼에 담긴 평균 속도 (초당 이동 거리) 계산
	glm::vec3 averageVelocity = m_accumulatedDisplacement / m_accumulatedTime;

	// 2. 목표 시간(targetDt) 동안 이동했을 거리를 반환
	return averageVelocity * targetDt;
}

void Velocitybuffer::Reset()
{
	m_accumulatedDisplacement = glm::vec3(0.0f);
	m_accumulatedTime = 0.0f;
}