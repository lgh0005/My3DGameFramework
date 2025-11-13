#include "EnginePch.h"
#include "SceneManager.h"

void SceneManager::LoadScene(const std::string& name)
{
	auto it = m_scenes.find(name);
	if (it == m_scenes.end())
	{
		SPDLOG_ERROR("Failed to find scene registration for name: {}", name);
		return;
	}

	m_activeScene.reset();

	m_activeScene = it->second();
	if (m_activeScene) SPDLOG_INFO("Successfully loaded scene: {}", name);
	else			   SPDLOG_ERROR("Failed to create scene instance: {}", name);
}

Scene* SceneManager::GetActiveScene() const
{
	return m_activeScene.get();
}
