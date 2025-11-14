#pragma once
#include <functional>
#include "Core/Scene.h"
#include "Core/Renderer.h"

struct ScenePackage
{
	SceneUPtr scene;
	RendererUPtr renderer;
};

class SceneManager
{
	DECLARE_SINGLE(SceneManager)

public:
	template<typename T_Scene, typename T_Renderer>
	void RegisterScene(const std::string& name);
	void LoadScene(const std::string& name);
	Scene* GetActiveScene() const;
	Renderer* GetActiveRenderer() const;

private:
	std::unordered_map<std::string, std::function<ScenePackage()>> m_scenes;
	ScenePackage m_activeScene;
};

/*=================================//
//   SceneManager template inlines //
//=================================*/
template<typename T_Scene, typename T_Renderer>
inline void SceneManager::RegisterScene(const std::string& name)
{
	m_scenes[name] = []()
	{
		SceneUPtr scene = T_Scene::Create();
		RendererUPtr renderer = T_Renderer::Create(WINDOW_WIDTH, WINDOW_HEIGHT);
		if (!scene || !renderer) return ScenePackage{ nullptr, nullptr };
		return ScenePackage{ std::move(scene), std::move(renderer) };
	};
}
