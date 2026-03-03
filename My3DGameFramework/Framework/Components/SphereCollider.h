#pragma once
#include "Collider.h"

CLASS_PTR(SphereCollider)
class SphereCollider : public Collider
{
	DEFINE_COMPONENT_TYPE(ComponentType::SphereCollider)

public:
	virtual ~SphereCollider();
	static SphereColliderUPtr Create(float radius = 1.0f);
	
	void SetRadius(float radius);
	float GetRadius() const { return m_radius; }

private:
	SphereCollider();
	bool Init(float radius);
	virtual JPH::ShapeRefC CreateRawShape() override;

	float m_radius	{ 1.0f };
};