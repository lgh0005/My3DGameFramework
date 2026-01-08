#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Transform)
CLASS_PTR(Component)
#pragma endregion

enum class GameObjectState
{
	Uninitialized, // 생성 직후
	Awake,         // Awake 호출 완료
	Active,        // Start 호출 완료 + 활성화 상태
	Inactive,      // 비활성화 상태 (SetActive(false))
	Dead           // Destroy 예약됨 (다음 프레임 삭제)
};

CLASS_PTR(GameObject)
class GameObject 
{
public:
	~GameObject();
	static GameObjectUPtr Create();

	Transform& GetTransform()    const	     { return *m_transform; }
	const std::string& GetName() const		 { return m_name; }
	void SetName(const std::string& name)	 { m_name = name; }

/*==========================================//
//   game object life-cycle state methods   //
//==========================================*/
public:
	// 상태 확인
	bool IsActive() const { return m_active; } // 로컬 활성화 상태
	bool IsActiveInHierarchy() const;          // 부모까지 포함한 실제 활성화 상태
	bool IsDead() const { return m_state == GameObjectState::Dead; }

	// 생명 주기 메서드
	void Awake();
	void Start();
	void SetActive(bool active);
	void FixedUpdate();
	void Update();
	void LateUpdate();
	void SetDestroy();
	void OnDestroy();
	void DontDestroyOnLoad(bool enable) { m_dontDestroyOnLoad = enable; }
	bool IsDontDestroyOnLoad() const { return m_dontDestroyOnLoad; }

/*=======================//
//   component methods   //
//=======================*/
public:
	void AddComponent(ComponentUPtr component);
	template<typename T> T* GetComponent() const;
	template<typename T> void GetComponentsInChildren(std::vector<T*>& outComponents);
	const std::vector<ComponentUPtr>& GetAllComponents() const { return m_components; }

/*=======================//
//   hierarchy methods   //
//=======================*/
public:
	GameObject* GetRoot();
	void SetParent(GameObject* parent);
	void AddChild(GameObject* child);
	void SetActiveStateHierarchy(bool active);

private:
	GameObject();
	bool Init();

	// 게임 오브젝트 기준 필요 멤버들
	Transform*		m_transform					 { nullptr };
	std::string		m_name						 { "GameObject" };
	bool			m_active					 { true };
	bool            m_dontDestroyOnLoad			 { false };
	GameObjectState m_state						 { GameObjectState::Uninitialized };
	
	// TODO : 개인적으로는 성능을 위해서라면 이것 또한
	// Registry를 따로 만드는 편이 더 좋아보이긴 한다.
	std::vector<ComponentUPtr> m_components;
};

/*=================================//
//   game object template inlines  //
//=================================*/
#pragma region GAME_OBJECT_INLINES
template<typename T>
inline T* GameObject::GetComponent() const
{
	// INFO : 중복된 컴포넌트 존재 시 먼저 발견한 컴포넌트를 반환
	ComponentType type = T::s_ComponentType;
	for (const auto& comp : m_components)
	{
		if (comp->MatchesType(type))
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
	for (Transform* child : GetTransform().GetChildren())
	{
		if (GameObject* childGO = child->GetOwner())
			childGO->GetComponentsInChildren<T>(outComponents);
	}
}
#pragma endregion
