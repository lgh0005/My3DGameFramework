#pragma once
#include <functional>
#include "Core/Scene.h"

class SceneManager
{
	DECLARE_SINGLE(SceneManager)
	using SceneFactory = std::function<SceneUPtr()>;

public:
	template<typename T>
	void RegisterScene(const std::string& name);

	void LoadScene
	(
		const std::string& scene, 		  
		const std::string& pipeline
	);
	Scene* GetActiveScene() const;
	void OnScreenResize(int32 width, int32 height);

	void Clear();

private:
	std::unordered_map<std::string, SceneFactory> m_scenes;
	SceneUPtr m_activeScene;
};

/*===================================//
//   SceneManager template inlines   //
//===================================*/
template<typename T>
inline void SceneManager::RegisterScene(const std::string& name)
{
	m_scenes[name] = []() -> SceneUPtr
	{
		return T::Create();
	};
}
