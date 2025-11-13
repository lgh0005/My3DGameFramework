#pragma once
#include <functional>
#include "Core/Scene.h"

class SceneManager
{
	DECLARE_SINGLE(SceneManager)

public:
	template<typename T>
	void RegisterScene(const std::string& name);
	void LoadScene(const std::string& name);
	Scene* GetActiveScene() const;

private:
	std::unordered_map<std::string, std::function<SceneUPtr()>> m_scenes;
	SceneUPtr m_activeScene;
};

/*=================================//
//   SceneManager template inlines //
//=================================*/
template<typename T>
inline void SceneManager::RegisterScene(const std::string& name)
{
	// 씬 팩토리 함수로 씬을 로드하는 방식
	m_scenes[name] = []() { return T::Create(); };
}
