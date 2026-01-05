#pragma once
#include "Collider.h"

CLASS_PTR(SphereCollider)
class SphereCollider : public Collider
{
public:
	virtual ~SphereCollider();
	static SphereColliderUPtr Create();
	static const ComponentType s_ComponentType = ComponentType::SphereCollider;
	virtual ComponentType GetComponentType() const override { return ComponentType::SphereCollider; }
	virtual JPH::ShapeRefC CreateShape() override;
	
	void SetRadius(float radius) { m_radius = radius; }
	float GetRadius() const { return m_radius; }

private:
	SphereCollider();
	bool Init();
	float m_radius{ 0.5f };
};