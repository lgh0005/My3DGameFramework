#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
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
	void SetOwner(GameObject* gameObject) { m_owner = gameObject; }
	GameObject* GetOwner() const { return m_owner; }

protected:
	GameObject* m_owner	   { nullptr };
};