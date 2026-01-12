#pragma once
#include "Misc/Enums.h"
#include "Object.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Transform)
CLASS_PTR(Component)
CLASS_PTR(Script)
#pragma endregion

CLASS_PTR(GameObject)
class GameObject : public Object
{
public:
	static constexpr usize INVALID_INSTANCE_ID = static_cast<InstanceID>(-1);
	static constexpr usize INVALID_SCENE_INDEX = static_cast<SceneIndex>(-1);

public:
	virtual ~GameObject();
	using RegistryIndex = usize;
	static GameObjectUPtr Create();

	InstanceID GetInstanceID() const { return m_instanceID; }
	void SetInstanceID(InstanceID id) { m_instanceID = id; }
	void SetSceneIndex(usize index) { m_sceneIndex = index; }
	RegistryIndex GetSceneIndex() const { return m_sceneIndex; }

	Transform& GetTransform()    const;
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
	void SetActive(bool active);

	// 생명 주기 메서드
	void Awake()		override;
	void Start()		override;
	void FixedUpdate()  override;
	void Update()		override;
	void LateUpdate()   override;
	void OnDestroy()	override;

	void SetDestroy();
	void DontDestroyOnLoad(bool enable) { m_dontDestroyOnLoad = enable; }
	bool IsDontDestroyOnLoad() const { return m_dontDestroyOnLoad; }

/*=======================//
//   component methods   //
//=======================*/
public:
	template<typename T> T* GetComponent() const;
	template<typename T> void GetComponentsInChildren(std::vector<T*>& outComponents);
	const std::vector<ComponentUPtr>& GetComponents() const { return m_components; }

public:
	template<typename T, typename... Args> T* AddComponent(Args&&... args);
	template <typename T> T* AddComponent(std::unique_ptr<T> component);

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

	// 상태
	bool			m_active					 { true };
	bool            m_dontDestroyOnLoad			 { false };
	GameObjectState m_state						 { GameObjectState::Uninitialized };
	
	// 컴포넌트
	std::vector<ComponentUPtr> m_components;
	std::array<Component*, (usize)ComponentType::MAX> m_componentCache;
	std::vector<Script*> m_scripts;

	// ObjectManager 상태 기록 캐시
	InstanceID m_instanceID						 { INVALID_INSTANCE_ID };
	SceneIndex m_sceneIndex						 { INVALID_SCENE_INDEX };
};

/*=================================//
//   game object template inlines  //
//=================================*/
#include "Object/GameObject.inl"
