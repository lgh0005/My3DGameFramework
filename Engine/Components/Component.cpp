#include "EnginePch.h"
#include "Component.h"
#include "Core/GameObject.h"
#include "Components/Transform.h"

bool Component::MatchesType(ComponentType type) const
{
	return GetComponentType() == type;
}

void Component::SetOwner(GameObject* gameObject)
{
	m_owner = gameObject;
}

GameObject* Component::GetOwner() const
{
	return m_owner;
}

Transform& Component::GetTransform() 
{
	return m_owner->GetTransform();
}

const Transform& Component::GetTransform() const
{
	return m_owner->GetTransform();
}