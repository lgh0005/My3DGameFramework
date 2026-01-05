#pragma once

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

	// Rendering
	Camera,
	DirectionalLight,
	SpotLight,
	PointLight,
	MeshOutline,
	SkinnedMeshRenderer,
	StaticMeshRenderer,
	InstancedMeshRenderer,

	// Audio
	AudioSource,
	AudioListener,

	// UI
	UICanvas,
	UIImage,
	UIText,

	// Collider
	BoxCollider,
	SphereCollider,

	Unknown
};

CLASS_PTR(Component)
class Component
{
	friend class GameObject;

public:
	virtual ~Component();
	virtual ComponentType GetComponentType() const = 0;
	bool MatchesType(ComponentType type) const;

	GameObject* GetOwner() const;
	Transform& GetTransform();
	const Transform& GetTransform() const;

/*========================================//
//   component life-cycle state methods   //
//========================================*/
public:
	bool IsActive() const;
	void SetEnable(bool enable);
	bool IsEnabled() const;

protected:
	virtual void Awake();			// 변수 초기화
	virtual void Start();			// 타 객체 참조

	virtual void FixedUpdate();	// 물리 연산
	virtual void Update();		// 게임 로직
	virtual void LateUpdate();	// 카메라/후처리

	virtual void OnEnable();		// 활성화 될 때
	virtual void OnDisable();		// 비활성화 될 때
	virtual void OnDestroy();		// 삭제 될 때

protected:
	Component();
	void SetOwner(GameObject* gameObject) { m_owner = gameObject; }
	GameObject* m_owner	   { nullptr };
	bool		m_enabled  { true };
};