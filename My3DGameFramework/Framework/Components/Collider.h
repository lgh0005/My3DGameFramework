#pragma once
#include "Object/Component.h"
#include <Jolt/Physics/Collision/Shape/Shape.h>

CLASS_PTR(Collider)
class Collider : public Component
{
	using Super = Component;
	DEFINE_COMPONENT_TYPE(ComponentType::Collider)

public:
	virtual ~Collider();
	virtual bool MatchesType(ComponentType type) const override;
	JPH::ShapeRefC CreateShape();

	JPH::ShapeRefC GetShape() const { return m_shape; }
	bool IsTrigger() const { return m_isTrigger; }
	const glm::vec3& GetOffset() const { return m_offset; }

	void SetTrigger(bool isTrigger) { m_isTrigger = isTrigger; }
	void SetOffset(const glm::vec3& offset);

protected:
	Collider();
	virtual JPH::ShapeRefC CreateRawShape() = 0;

	bool m_isTrigger		{ false };
	glm::vec3 m_offset = glm::vec3(0.0f);
	JPH::ShapeRefC m_shape	{ nullptr };
};