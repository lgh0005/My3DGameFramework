#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
CLASS_PTR(Transform)
#pragma endregion

enum class ComponentType
{
    Animator,
	Camera,
	DirectionalLight,
	SpotLight,
	PointLight,
	Transform,
	Script,
	AudioSource,
	AudioListener,
	MeshOutline,
	SkinnedMeshRenderer,
	StaticMeshRenderer,
	InstancedMeshRenderer
};

CLASS_PTR(Component)
class Component
{
public:
	virtual ~Component();
	virtual ComponentType GetComponentType() const = 0;
	bool MatchesType(ComponentType type) const;

	void SetOwner(GameObject* gameObject);
	GameObject* GetOwner() const;
	Transform& GetTransform();
	const Transform& GetTransform() const;

/*========================================//
//   component life-cycle state methods   //
//=========================================*/
public:
	// TODO : 추가해야 할 메서드
	// 0. Awake
	// 1. Start
	// 2. FixedUpdate
	// 3. Update
	// 4. LateUpdate
	
	// TODO : 자신을 소유하는 게임 오브젝트 상태를
	// 추적해서 상태를 추적하는 메서드를 작성
	// 1. Destroy
	// 2. SetActive

protected:
	Component();
	GameObject* m_owner	   { nullptr };
};