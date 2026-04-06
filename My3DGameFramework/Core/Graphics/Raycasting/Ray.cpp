#include "EnginePch.h"
#include "Ray.h"
#include "Object/GameObject.h"
#include "Components/Camera.h"

Ray::Ray() : m_origin(0.f), m_direction(0.f, 0.f, -1.f) {}
Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) : m_origin(origin)
{
    // 1. 방향 벡터가 유효한 길이(0이 아님)를 가지는지 확인
    if (Utils::HasLength(direction)) m_direction = Utils::SafeNormalize(direction);
    else  m_direction = glm::vec3(0.f, 0.f, -1.f);
}

glm::vec3 Ray::GetPoint(float distance) const
{
	return m_origin + (m_direction * distance);
}


