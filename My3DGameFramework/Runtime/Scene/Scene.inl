#pragma once
#include "Scene/Scene.h"             
#include "Scene/ComponentRegistry.h"
#include "Object/GameObject.h"

template<typename T>
inline GameObject* Scene::FindObjectByType()
{
	ComponentRegistry* compReg = GetComponentRegistry();
	if (!compReg) return nullptr;

	const auto& components = compReg->GetComponents<T>();

	// Active 상태인 첫 번째 오브젝트 반환
	for (auto* comp : components)
	{
		if (comp && comp->IsActive())
		{
			return comp->GetOwner();
		}
	}

	return nullptr;
}

template<typename T>
inline std::vector<GameObject*> Scene::FindObjectsByType()
{
	ComponentRegistry* compReg = GetComponentRegistry();
	if (!compReg) return {};

	const auto& components = compReg->GetComponents<T>();
	std::vector<GameObject*> objects;
	objects.reserve(components.size());

	for (auto* comp : components)
	{
		if (comp && comp->IsActive())
		{
			objects.push_back(comp->GetOwner());
		}
	}

	return objects;
}