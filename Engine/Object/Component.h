#pragma once
#include "Object.h"
#include "Managers/ObjectManager.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
CLASS_PTR(Transform)
#pragma endregion

enum class ComponentType
{
	// Core
	Transform,
	Script,
	Animator,
	Rigidbody,
	Camera,

	// Rendering
	MeshRenderer,
	SkinnedMeshRenderer,
	StaticMeshRenderer,
	InstancedMeshRenderer,
	MeshOutline,

	// Light
	Light,
	DirectionalLight,
	SpotLight,
	PointLight,

	// Audio
	AudioSource,
	AudioListener,

	// UI
	UICanvas,
	UIImage,
	UIText,

	// Collider
	Collider,
	BoxCollider,
	SphereCollider,
	CapsuleCollider,

	// Misc
	Terrain,
	MAX
};

enum class ComponentState
{
	Uninitialized,  // 생성자 호출 직후 (아직 Awake/Start 안 함)
	Initialized,    // Awake 완료 (데이터 세팅 완료)
	Running,        // Start 완료 (이제부터 정상적으로 Update 돔) -> 기존의 Active 대체
	Dead            // 사망 선고 (메모리 해제 대기)
};

CLASS_PTR(Component)
class Component : public Object
{
	friend class GameObject;

public:
	virtual ~Component();
	virtual ComponentType GetComponentType() const = 0;
	virtual bool MatchesType(ComponentType type) const;

	void SetRegistryIndex(usize idx) { m_registryIndex = idx; }
	usize GetRegistryIndex() const { return m_registryIndex; }

	GameObject* GetOwner() const;
	Transform& GetTransform();
	const Transform& GetTransform() const;

/*========================================//
//   component life-cycle state methods   //
//========================================*/
public:
	ComponentState GetState() const { return m_state; }
	bool IsActive() const;
	void SetEnable(bool enable);
	bool IsEnabled() const;

protected:
	virtual void Awake()		override;			// 변수 초기화
	virtual void Start()		override;			// 타 객체 참조
	virtual void FixedUpdate()	override;			// 물리 연산
	virtual void Update()		override;			// 게임 로직
	virtual void LateUpdate()	override;			// 카메라/후처리
	virtual void OnDestroy()	override;			// 삭제 될 때

	virtual void OnEnable();						// 활성화 될 때
	virtual void OnDisable();						// 비활성화 될 때

protected:
	Component();
	void SetOwner(GameObject* gameObject) { m_owner = gameObject; }
	GameObject* m_owner	   { nullptr };
	bool		m_enabled  { true };
	ComponentState m_state { ComponentState::Uninitialized };
	RegistryIndex  m_registryIndex	{ static_cast<usize>(-1) };
};