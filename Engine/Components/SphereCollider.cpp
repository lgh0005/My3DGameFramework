#include "EnginePch.h"
#include "SphereCollider.h"
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include "CapsuleCollider.h"

DECLARE_DEFAULTS_IMPL(SphereCollider)

SphereColliderUPtr SphereCollider::Create(float radius)
{
	auto collider = SphereColliderUPtr(new SphereCollider());
	if (!collider->Init(radius)) return nullptr;
	return std::move(collider);
}

bool SphereCollider::Init(float radius)
{
	m_radius = radius;
	m_shape = CreateShape();
	if (!m_shape) return false;
	return true;
}

JPH::ShapeRefC SphereCollider::CreateRawShape()
{
	float safeRadius = glm::max<float>(m_radius * 0.5f, 0.001f);
	return new JPH::SphereShape(safeRadius);
}

void SphereCollider::SetRadius(float radius)
{
	if (m_radius == radius) return;
	m_radius = radius;
	m_shape = CreateShape();
}

