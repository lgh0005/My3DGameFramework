#include "EnginePch.h"
#include "SphereCollider.h"
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

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

JPH::ShapeRefC SphereCollider::CreateShape()
{
	// 1. SphereShapeSettings 생성 (인자로 반지름 전달)
	JPH::SphereShapeSettings settings(m_radius * 0.5f);

	// 2. Shape 생성 시도 (결과는 Result로 나옴)
	JPH::ShapeSettings::ShapeResult result = settings.Create();

	// 3. 에러 체크 및 반환
	if (result.HasError())
	{
		LOG_ERROR("Failed to create Sphere Shape: {}", result.GetError().c_str());
		return nullptr;
	}

	// 4. 성공 시 ShapeRefC 반환
	return result.Get();
}

void SphereCollider::SetRadius(float radius)
{
	if (m_radius == radius) return;
	m_radius = radius;
	m_shape = CreateShape();
}
