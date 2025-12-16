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
	MeshRenderer,
	Script,
	AudioSource,
	AudioListener
};

CLASS_PTR(Component)
class Component
{
public:
	virtual ~Component() = default;
	virtual ComponentType GetComponentType() const = 0;
	bool MatchesType(ComponentType type) const;

	void SetOwner(GameObject* gameObject);
	GameObject* GetOwner() const;
	Transform& GetTransform();
	const Transform& GetTransform() const;

protected:
	GameObject* m_owner	   { nullptr };
};