#include "RuntimePch.h"
#include "SceneManager.h"
#include "Scene/Scene.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	SceneManager::SceneManager() = default;
	SceneManager::~SceneManager() = default;

	void SceneManager::Update()
	{
		if (!m_activeScene) return;
		m_activeScene->Update();
	}

	void SceneManager::Shutdown()
	{
		m_activeScene.reset();
		m_scenes.clear();
	}
}