#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
CLASS_PTR(Transform)
#pragma endregion

enum class ComponentType
{
    Animator,
	Camera,
	Light,
	Transform,
	MeshRenderer
};

CLASS_PTR(Component)
class Component
{
public:
	virtual ~Component() = default;
	virtual ComponentType GetType() const = 0;
	void SetOwner(GameObject* gameObject);
	GameObject* GetOwner() const;
	Transform& GetTransform() const;

protected:
	GameObject* m_owner	   { nullptr };
};