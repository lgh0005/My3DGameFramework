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

	const std::string& GetName() const { return m_name; }
	uint32 GetNameHash() const { return m_nameHash; }
	void SetName(const std::string& name);

/*=============================================//
//   GameObject life-cycle methods by engine   //
//=============================================*/
public:
	void Awake()		override final;
	void Start()		override final;
	void FixedUpdate()  override final;
	void Update()		override final;
	void LateUpdate()   override final;
	void OnDestroy()	override final;

/*===================================//
//   GameObject activation methods   //
//===================================*/
public:
	bool GetLocalActive() const;  // 로컬 활성화 상태
	bool IsActive() const;        // 부모까지 포함한 모델 트리의 실제 활성화 상태
	void SetActive(bool active);  // 재귀 전파 포함하는 활성화/비활성화 설정

/*====================================//
//   GameObject destruction methods   //
//====================================*/
public:
	void SetDestroy();
	void DontDestroyOnLoad(bool enable) { m_dontDestroyOnLoad = enable; }
	bool IsDontDestroyOnLoad() const { return m_dontDestroyOnLoad; }

/*===================================//
//   GameObject state checking API   //
//===================================*/
public:
	void SetGameObjectState(GameObjectState state) { m_state = state; }
	bool IsAwake() const { return m_state >= GameObjectState::Awake; }
	bool IsStarted() const { return m_state >= GameObjectState::Started; }
	bool IsDead() const { return m_state == GameObjectState::Dead; }

/*=======================//
//   component methods   //
//=======================*/
public:
	template<typename T, typename... Args> T* AddComponent(Args&&... args);
	template <typename T> T* AddComponent(std::unique_ptr<T> component);
	template<typename T> T* GetComponent() const;
	template<typename T> void GetComponentsInChildren(std::vector<T*>& outComponents);
	const std::vector<ComponentUPtr>& GetComponents() const { return m_components; }

/*=======================//
//   hierarchy methods   //
//=======================*/
public:
	GameObject* GetRoot();
	void SetParent(GameObject* parent);
	void AddChild(GameObject* child);

private:
	GameObject();
	bool Init();
	void SetActiveState(bool active); // 실제로 켜지거나 꺼졌을 때 자식들과 컴포넌트에게 알림
	void EnsureInitialized(); // 상태 무결성 보장 메서드 : Awake나 Start가 누락된 경우 강제로 수행

	// 게임 오브젝트의 상태
	bool			m_active					 { true };
	bool            m_dontDestroyOnLoad			 { false };
	GameObjectState m_state						 { GameObjectState::Uninitialized };
	
	// 게임 오브젝트가 소유하는 컴포넌트 캐시들
	std::vector<ComponentUPtr> m_components;
	std::array<Component*, (usize)ComponentType::MAX> m_componentCache;
	std::vector<Script*> m_scripts;

	// ObjectManager상에서의 게임 오브젝트 상태 기록 캐시
	std::string m_name;
	uint32      m_nameHash						{ 0 };
	InstanceID  m_instanceID					{ INVALID_INSTANCE_ID };
	SceneIndex  m_sceneIndex					{ INVALID_SCENE_INDEX };
};

/*=================================//
//   game object template inlines  //
//=================================*/
#include "Object/GameObject.inl"
