#pragma once
#include "Component.h"
#include <Jolt/Physics/Collision/Shape/Shape.h>

CLASS_PTR(Collider)
class Collider : public Component
{
	using Super = Component;

public:
	virtual ~Collider();
	static const ComponentType s_ComponentType = ComponentType::Collider;
	virtual bool MatchesType(ComponentType type) const override;
	virtual JPH::ShapeRefC CreateShape() = 0;

	void SetTrigger(bool isTrigger) { m_isTrigger = isTrigger; }
	bool IsTrigger() const { return m_isTrigger; }

protected:
	Collider();

	bool m_isTrigger = false;
};