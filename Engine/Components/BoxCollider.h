#pragma once
#include "Collider.h"

CLASS_PTR(BoxCollider)
class BoxCollider : public Collider
{
public:
	virtual ~BoxCollider();
	static BoxColliderUPtr Create();
	static const ComponentType s_ComponentType = ComponentType::BoxCollider;
	virtual ComponentType GetComponentType() const override { return ComponentType::BoxCollider; }
	virtual JPH::ShapeRefC CreateShape() override;

private:
	BoxCollider();
	bool Init();
	glm::vec3 m_size{ 1.0f };
};