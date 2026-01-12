#pragma once
#include "Object/GameObject.h"
#include "Object/Component.h"
#include "Components/Script.h"
#include "Components/Transform.h"

// 1. 팩토리 래퍼 (Factory Wrapper)
template<typename T, typename... Args>
inline T* GameObject::AddComponent(Args&&... args)
{
	auto compUPtr = T::Create(std::forward<Args>(args)...);
	if (!compUPtr) nullptr;
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

	// 3. 소유권 이동
	m_components.push_back(std::move(component));

	// 4. ObjectManager 등록
	OBJECT.RegisterComponent(basePtr);

	// 5. 런타임 초기화 (Awake/Start)
	// TODO : 이쪽은 보다 정교한 상태 판별을 바탕으로 좀 더 상태에 맞는
	// 동작을 작성할 필요가 있음
	if (m_state >= GameObjectState::Awake)
		basePtr->Awake();

	if (m_state >= GameObjectState::Active && IsActiveInHierarchy())
	{
		if (basePtr->IsEnabled())
		{
			basePtr->OnEnable();
			basePtr->Start();
		}
	}

	return compPtr;
}

template<typename T>
inline T* GameObject::GetComponent() const
{
	// 1. 스크립트인 경우
	if constexpr (std::is_base_of<Script, T>::value)
	{
		ScriptID id = ScriptRegistry::GetID<T>();
		for (Script* script : m_scripts)
		{
			if (script->GetScriptID() == id)
				return static_cast<T*>(script);
		}
		return nullptr;
	}

	// 2. 그 외 일반적인 컴포넌트인 경우
	else
	{
		usize index = static_cast<usize>(T::s_ComponentType);

		// [1차 시도] O(1) 캐시 조회 (Transform 등은 여기서 즉시 리턴됨)
		Component* cached = m_componentCache[index];
		if (cached != nullptr) return static_cast<T*>(cached);

		// [2차 시도 - 해결책] 캐시에 없다면? (예: Collider 요청했는데 BoxCollider만 있는 경우)
		// 기존의 O(N) 방식으로 전체를 뒤져서 찾습니다.
		ComponentType type = T::s_ComponentType;
		for (const auto& comp : m_components)
		{
			if (comp->MatchesType(type))
				return static_cast<T*>(comp.get());
		}

		return nullptr;
	}
}

template<typename T>
inline void GameObject::GetComponentsInChildren(std::vector<T*>& outComponents)
{
	// 1. 나 자신에게 있는지 확인
	T* comp = GetComponent<T>();
	if (comp) outComponents.push_back(comp);

	// 2. 자식들에게 재귀 호출
	for (Transform* child : GetTransform().GetChildren())
	{
		if (GameObject* childGO = child->GetOwner())
			childGO->GetComponentsInChildren<T>(outComponents);
	}
}