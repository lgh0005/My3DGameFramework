#include "EnginePch.h"
#include "Collider.h"
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

DECLARE_DEFAULTS_IMPL(Collider)

bool Collider::MatchesType(ComponentType type) const
{
	return type == ComponentType::Collider || Super::MatchesType(type);
}

JPH::ShapeRefC Collider::CreateShape()
{
	// 1. 자식 클래스에게 "순수한 모양"을 만들어오라고 시킴
	JPH::ShapeRefC rawShape = CreateRawShape();
	if (rawShape == nullptr) return nullptr;

	if (Utils::HasLength(m_offset))
	{
		JPH::RotatedTranslatedShapeSettings offsetSettings
		(
			Utils::ToJoltVec3(m_offset),
			JPH::Quat::sIdentity(),
			rawShape
		);

		JPH::ShapeSettings::ShapeResult result = offsetSettings.Create();
		if (result.HasError())
		{
			LOG_ERROR("Failed to create Offset Shape: {}", result.GetError().c_str());
			return rawShape;
		}

		return result.Get();
	}

	return rawShape;
}

void Collider::SetOffset(const glm::vec3& offset)
{
	if (m_offset == offset) return;
	m_offset = offset;
	m_shape = CreateShape();
}