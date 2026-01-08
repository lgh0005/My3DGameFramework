#pragma once
#include "Collider.h"

CLASS_PTR(CapsuleCollider)
class CapsuleCollider : public Collider
{
public:
	virtual ~CapsuleCollider();
	static CapsuleColliderUPtr Create(float radius = 0.5f, float height = 2.0f);
	static const ComponentType s_ComponentType = ComponentType::CapsuleCollider;
	virtual ComponentType GetComponentType() const override { return ComponentType::CapsuleCollider; }

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