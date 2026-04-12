#pragma once
#include "Scene/Scene.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Scene)

	class SceneManager
	{
		MGF_DECLARE_SINGLE(SceneManager)
		using SceneFactory = Func<SceneUPtr>;

	private:
		SceneManager();
		~SceneManager();

	public:
		void Update();
		void Shutdown();

	public:
		Scene* GetActiveScene() const { return m_activeScene.get(); }
		template<typename T> void RegisterScene(StringView name);
		template<typename T> void LoadScene();

	public:
		HashMap<int16, SceneFactory> m_scenes;
		SceneUPtr m_activeScene;
	};
}

#include "Managers/SceneManager.inl"