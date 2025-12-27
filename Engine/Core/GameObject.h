#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Transform)
CLASS_PTR(Component)
#pragma endregion

CLASS_PTR(GameObject)
class GameObject 
{
public:
	static GameObjectUPtr Create();

	// 컴포넌트 메서드들
	void AddComponent(ComponentUPtr component);
	template<typename T>
	T* GetComponent() const;
	template<typename T>
	T* GetComponentInParent() const;

	// 필요 멤버들의 getter와 setter들
	Transform& GetTransform()    const	     { return *m_transform; }
	const std::string& GetName() const		 { return m_name; }
	void SetName(const std::string& name)	 { m_name = name; }
	const std::vector<ComponentUPtr>& GetAllComponents() 
								 const		 { return m_components; }

	// 계층 구조 메서드들
	void SetParent(GameObject* parent);
	void AddChild(GameObject* child);
	GameObject* GetChildByIndex(usize index);
	GameObject* GetChildByName(const std::string& name);

	// 게임 오브젝트가 가져야 할 기본 메서드들
	// TODO : 추가해야 할 메서드
	// 0. Find
	// 1. Destroy
	// 2. SetActive
	// 3. DontDestroyOnLoad
	// 4. Instantiate

private:
	GameObject() = default;
	bool Init();

	// 게임 오브젝트 기준 필요 멤버들
	Transform*		m_transform					 { nullptr };
	std::string		m_name						 { "GameObject" };
	bool			m_active					 { true };
	std::vector<ComponentUPtr> m_components;
};

/*=================================//
//   game object template inlines  //
//=================================*/
#pragma region GAME_OBJECT_INLINES
template<typename T>
inline T* GameObject::GetComponent() const
{
	ComponentType type = T::s_ComponentType;
	for (const auto& comp : m_components)
	{
		if (comp->MatchesType(type))
			return static_cast<T*>(comp.get());
	}
	return nullptr;
}

template<typename T>
inline T* GameObject::GetComponentInParent() const
{
	// 1. 나 자신에게 있는지 먼저 확인
	T* comp = GetComponent<T>();
	if (comp) return comp;

	// 2. 부모 탐색 시작
	Transform* current = m_transform->GetParent();
	while (current != nullptr)
	{
		GameObject* parentGO = current->GetOwner();
		if (parentGO)
		{
			comp = parentGO->GetComponent<T>();
			if (comp) return comp;
		}
		// 계속 위로 올라감
		current = current->GetParent();
	}
}
#pragma endregion
