#pragma once
#include "Collider.h"

CLASS_PTR(CapsuleCollider)
class CapsuleCollider : public Collider
{
	DEFINE_COMPONENT_TYPE(ComponentType::CapsuleCollider)

public:
	virtual ~CapsuleCollider();
	static CapsuleColliderUPtr Create(float radius = 0.5f, float height = 2.0f);

	void SetRadius(float radius);
	float GetRadius() const { return m_radius; }
	void SetHeight(float height);
	float GetHeight() const { return m_height; }

private:
	CapsuleCollider();
	bool Init(float radius, float height);
	virtual JPH::ShapeRefC CreateRawShape() override;

	float m_radius{ 0.5f };
	float m_height{ 2.0f };
};