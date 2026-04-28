#include "CorePch.h"
#include "Mechanic.h"
#include "Entities/Component.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	Mechanic::Mechanic(Component* owner) : m_owner(owner) { }
	Mechanic::~Mechanic() = default;
	Mechanic::Mechanic(Mechanic&& other) noexcept = default;
	Mechanic& Mechanic::operator=(Mechanic&& other) noexcept = default;

	void Mechanic::SetOwner(Component* owner)
	{
		m_owner = owner;
	}

	Component* Mechanic::GetOwner() const
	{
		return m_owner;
	}

	/*============================//
	//    Mechanic Custom Type    //
	//============================*/
	int16 Mechanic::s_typeIndex = -1;
	const MGFType* Mechanic::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Mechanics");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}
}