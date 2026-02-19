#include "EnginePch.h"
#include "BoxCollider.h"
#include "Object/GameObject.h"
#include "Components/Transform.h"
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

DECLARE_DEFAULTS_IMPL(BoxCollider)

BoxColliderUPtr BoxCollider::Create(const glm::vec3& size)
{
	auto collider = BoxColliderUPtr(new BoxCollider());
	if (!collider->Init(size)) return nullptr;
	return std::move(collider);
}

bool BoxCollider::Init(const glm::vec3& size)
{
	m_size = size;
	m_shape = CreateShape();
	if (!m_shape) return false;
	return true;
}

JPH::ShapeRefC BoxCollider::CreateRawShape()
{
	// 1. Half Extents 계산
	glm::vec3 halfExtent = m_size * 0.5f;

	// 2. 최소 크기 보정 (Utils 사용)
	// 길이 비교가 아니라, X, Y, Z 각각 최소 0.001f는 되도록 강제함
	halfExtent = Utils::GlmVec3Max(halfExtent, glm::vec3(0.001f));

	// 3. Jolt BoxShape 생성
	return new JPH::BoxShape(JPH::Vec3(halfExtent.x, halfExtent.y, halfExtent.z));
}

