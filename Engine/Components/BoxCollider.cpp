#include "EnginePch.h"
#include "BoxCollider.h"
#include "Scene/GameObject.h"
#include "Components/Transform.h"
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

DECLARE_DEFAULTS_IMPL(BoxCollider)

BoxColliderUPtr BoxCollider::Create()
{
	auto collider = BoxColliderUPtr(new BoxCollider());
	if (!collider->Init()) return nullptr;
	return std::move(collider);
}

bool BoxCollider::Init()
{
	auto shape = CreateShape();
	if (!shape) return false;
	return true;
}

JPH::ShapeRefC BoxCollider::CreateShape()
{
	// 1. 로컬 사이즈
	glm::vec3 finalExtent = m_size;

	// 2. 주인님(GameObject)이 있으면 월드 스케일 반영 (Just-In-Time)
	// Rigidbody가 호출하는 시점에는 주인이 있으므로 이 코드가 실행됨!
	if (GetOwner())
	{
		glm::vec3 worldScale = GetOwner()->GetTransform().GetWorldScale();
		finalExtent *= glm::abs(worldScale);
	}
	
	// 1. BoxShapeSettings 생성 (Half Extent 주의)
	JPH::BoxShapeSettings settings(Utils::ToJoltVec3(finalExtent * 0.5f));

	// 2. Shape 생성 시도 (결과는 Result로 나옴)
	JPH::ShapeSettings::ShapeResult result = settings.Create();

	// 3. 에러 체크 및 반환
	if (result.HasError())
	{
		LOG_ERROR("Failed to create Box Shape: {}", result.GetError().c_str());
		return nullptr;
	}

	// 4. 성공 시 ShapeRefC 반환
	return result.Get();
}
