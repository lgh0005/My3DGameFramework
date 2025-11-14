#include "EnginePch.h"
#include "Component.h"
#include "Core/GameObject.h"
#include "Components/Transform.h"

void Component::SetOwner(GameObject* gameObject)
{
	m_owner = gameObject;
}

GameObject* Component::GetOwner() const
{
	return m_owner;
}

Transform& Component::GetTransform() const 
{
	return m_owner->GetTransform();
}