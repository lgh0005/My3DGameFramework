#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(ComponentRegistry)
CLASS_PTR(GameObjectRegistry)
#pragma endregion

CLASS_PTR(SceneRegistry)
class SceneRegistry
{
public:
	~SceneRegistry();
	static SceneRegistryUPtr Create();
	ComponentRegistry* GetComponentRegistry() const;
	GameObjectRegistry* GetGameObjectRegistry() const;

private:
	SceneRegistry();
	bool Init();
	ComponentRegistryUPtr   m_componentRegistry;
	GameObjectRegistryUPtr	m_gameObjectRegistry;
};