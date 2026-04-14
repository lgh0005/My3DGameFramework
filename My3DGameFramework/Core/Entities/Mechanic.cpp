#include "CorePch.h"
#include "Mechanic.h"
#include "Entities/Component.h"

namespace MGF3D
{
	Mechanic::Mechanic(Component* owner) : m_owner(owner) { }
	Mechanic::~Mechanic() = default;

	void Mechanic::SetOwner(Component* owner)
	{
		m_owner = owner;
	}

	Component* Mechanic::GetOwner() const
	{
		return m_owner;
	}
}