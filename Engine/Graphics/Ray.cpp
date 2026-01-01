#include "EnginePch.h"
#include "Ray.h"
#include "Scene/GameObject.h"
#include "Components/Camera.h"

Ray::Ray() : m_origin(0.f), m_direction(0.f, 0.f, -1.f) {}
Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) : m_origin(origin)
{
	// 방향 벡터 길이가 0에 가까우면 정규화 시 NaN이 뜨므로 안전장치
	if (glm::length2(direction) < glm::epsilon<float>()) m_direction = glm::vec3(0.f, 0.f, -1.f);
	else m_direction = glm::normalize(direction);
}

glm::vec3 Ray::GetPoint(float distance) const
{
	return m_origin + (m_direction * distance);
}


