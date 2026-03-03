#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(ComponentRegistry)
	MGF_CLASS_PTR(GameObjectRegistry)

	MGF_CLASS_PTR(SceneRegistry)
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
}
