#include "EnginePch.h"
#include "GameObject.h"
#include "Scene/Scene.h"
#include "Components/Component.h"
#include "Components/Transform.h"

GameObject::GameObject() = default;
GameObject::~GameObject() = default;

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

/*==========================//
//  hierarchical structure  //
//==========================*/
void GameObject::AddComponent(ComponentUPtr component)
{
	component->SetOwner(this);
	m_components.push_back(std::move(component));
}

void GameObject::SetParent(GameObject* parent)
{
	// parent가 nullptr이면 부모를 없앤다(Root로 만든다)는 의미
	Transform* parentTransform = (parent != nullptr) ? &parent->GetTransform() : nullptr;
	
	// Transform에게 실제 로직 위임
	m_transform->SetParent(parentTransform);
}

void GameObject::AddChild(GameObject* child)
{
	if (child == nullptr) return;

	// "너 내 자식이 돼라" == "너의 부모는 나다"
	child->SetParent(this);
}

GameObject* GameObject::GetChildByIndex(usize index)
{
	return m_transform->GetChildGameObjectByIndex(index);
}

GameObject* GameObject::GetChildByName(const std::string& name)
{
	return m_transform->GetChildGameObjectByName(name);
}