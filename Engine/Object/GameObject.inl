#pragma once
#include "Object/GameObject.h"
#include "Object/Component.h"
#include "Components/Script.h"
#include "Components/Transform.h"
#include "Scene/ComponentRegistry.h"

// 1. 팩토리 래퍼 (Factory Wrapper)
template<typename T, typename... Args>
inline T* GameObject::AddComponent(Args&&... args)
{
	auto compUPtr = T::Create(std::forward<Args>(args)...);
	if (!compUPtr) return nullptr;
	return AddComponent(std::move(compUPtr));
}

// 2. 핵심 로직 (Core Logic)
template<typename T>
inline T* GameObject::AddComponent(std::unique_ptr<T> component)
{
	static_assert(std::is_base_of<Component, T>::value);

	if (!component) return nullptr;

	// 1. 소유권 이동 전 포인터 획득
	T* compPtr = component.get();
	Component* basePtr = compPtr;
	basePtr->SetOwner(this);

	// 2. 내부 캐시(m_componentCache 또는 m_scripts) 갱신
	ComponentType type = compPtr->GetComponentType();

	switch (type)
	{
		case ComponentType::Script:
		{
			m_scripts.push_back(static_cast<Script*>(basePtr));
			break;
		}

		default:
		{
			usize index = static_cast<usize>(type);
			if (index < m_componentCache.size())
			{
				// 비어있으면 채움 (Transform 등은 여기서 캐싱)
				if (m_componentCache[index] == nullptr)
					m_componentCache[index] = basePtr;
			}
			break;
		}
	}

	// 3. 소유권 이동 및 등록
	m_components.push_back(std::move(component));
	RegisterComponentToScene(component.get());

	// 5. 런타임 상태 동기화 (Catch-up)
	// 컴포넌트가 추가되는 시점의 GameObject 상태에 맞춰, 컴포넌트의 생명주기를 따라잡습니다.

	// 5-1. 게임오브젝트가 이미 깨어난 상태라면 컴포넌트도 Awake
	if (IsAwake()) basePtr->Awake();

	// 5-2. 게임오브젝트가 이미 시작된 상태라면 컴포넌트도 Start
	if (IsStarted()) basePtr->Start();

	// 5-2. 게임오브젝트가 활성화 상태이고, 컴포넌트도 켜져있다면 OnEnable
	if (IsActive() && basePtr->IsEnabled()) basePtr->OnEnable();

	return compPtr;
}

template<typename T>
inline T* GameObject::GetComponent() const
{
	static_assert(std::is_base_of<Component, T>::value);

	usize index = static_cast<usize>(T::s_ComponentType);

	// 1. O(1) 캐시 조회
	if (index < m_componentCache.size())
	{
		Component* cached = m_componentCache[index];
		if (cached != nullptr) return static_cast<T*>(cached);
	}

	// 2. 캐시에 없는 경우
	ComponentType requestedType = T::s_ComponentType;
	for (const auto& comp : m_components)
	{
		if (comp->MatchesType(requestedType))
			return static_cast<T*>(comp.get());
	}

	return nullptr;
}

template<typename T>
inline void GameObject::GetComponentsInChildren(std::vector<T*>& outComponents)
{
	// 1. 나 자신에게 있는지 확인
	T* comp = GetComponent<T>();
	if (comp) outComponents.push_back(comp);

	// 2. 자식들에게 재귀 호출
	// TODO : 이후에 flat한 호출로 로직 수정 필요
	for (Transform* child : GetTransform().GetChildren())
	{
		if (GameObject* childGO = child->GetOwner())
			childGO->GetComponentsInChildren<T>(outComponents);
	}
}