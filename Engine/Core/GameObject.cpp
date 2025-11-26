#include "EnginePch.h"
#include "GameObject.h"
#include "Core/Scene.h"
#include "Components/Component.h"
#include "Components/Transform.h"

GameObjectUPtr GameObject::Create()
{
	auto gameObject = std::unique_ptr<GameObject>(new GameObject());
	if (!gameObject->Init()) return nullptr;
	return gameObject;
}

bool GameObject::Init()
{
	// 기본적으로 Transform을 소유
	auto transform = Transform::Create();
	if (!transform) return false;
	m_transform = transform.get();
	AddComponent(std::move(transform));
	return true;
}

void GameObject::AddComponent(ComponentUPtr component)
{
	component->SetOwner(this);
	m_components.push_back(std::move(component));
}