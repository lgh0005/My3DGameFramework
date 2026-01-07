#include "EnginePch.h"
#include "BoxCollider.h"
#include "Scene/GameObject.h"
#include "Components/Transform.h"
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

DECLARE_DEFAULTS_IMPL(BoxCollider)

BoxColliderUPtr BoxCollider::Create(const glm::vec3& size, const glm::vec3& center)
{
	auto collider = BoxColliderUPtr(new BoxCollider());
	if (!collider->Init(size, center)) return nullptr;
	return std::move(collider);
}

bool BoxCollider::Init(const glm::vec3& size, const glm::vec3& center)
{
	m_size = size;
	m_center = center;
	auto shape = CreateShape();
	if (!shape) return false;
	return true;
}

JPH::ShapeRefC BoxCollider::CreateShape()
{	
	// 1. BoxShapeSettings 생성 (Half Extent 주의)
	JPH::BoxShapeSettings settings(Utils::ToJoltVec3(m_size * 0.5f));

	// 2. Shape 생성 시도 (결과는 Result로 나옴)
	JPH::ShapeSettings::ShapeResult result = settings.Create();
	if (result.HasError())
	{
		LOG_ERROR("Failed to create Box Shape: {}", result.GetError().c_str());
		return nullptr;
	}

	JPH::ShapeRefC finalShape = result.Get();
	if (m_center != glm::vec3(0.0f))
	{
		JPH::RotatedTranslatedShapeSettings offsetSettings
		(
			Utils::ToJoltVec3(m_center),    // 이동할 위치 (Offset)
			JPH::Quat::sIdentity(),         // 회전은 그대로
			finalShape                      // 알맹이 (박스)
		);

		JPH::ShapeSettings::ShapeResult offsetResult = offsetSettings.Create();
		if (offsetResult.HasError())
		{
			LOG_ERROR("Failed to create Offset Shape: {}", offsetResult.GetError().c_str());
			return nullptr;
		}

		finalShape = offsetResult.Get();
	}

	return finalShape;
}
