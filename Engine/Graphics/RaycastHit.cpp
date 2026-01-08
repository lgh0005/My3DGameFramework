#include "EnginePch.h"
#include "RaycastHit.h"
#include "GameObject/GameObject.h"

RaycastHit::RaycastHit()
{
	Reset();
}
RaycastHit::~RaycastHit() = default;

void RaycastHit::Reset()
{
	m_gameObject = nullptr;
	m_point = glm::vec3(0.f);
	m_normal = glm::vec3(0.f);
	m_distance = 0.f;
	m_isHit = false;
}

void RaycastHit::SetHitResult
(
	GameObject* obj, 
	const glm::vec3& point, const glm::vec3& normal, 
	float dist
)
{
	m_gameObject = obj;
	m_point = point;
	m_normal = normal;
	m_distance = dist;
	m_isHit = true;
}
