#include "EnginePch.h"
#include "SceneManager.h"

void SceneManager::LoadScene
(
	const std::string& scene,
	const std::string& pipeline
)
{
	// 1. 등록된 씬을 먼저 찾기
	auto it = m_scenes.find(scene);
	if (it == m_scenes.end())
	{
		SPDLOG_ERROR("Failed to find scene registration for name: {}", scene);
		return;
	}

	// 2. 파이프 라인 교체
	RENDER.SetPipeline(pipeline);

	// 3. 기존 씬 정리
	m_activeScene.reset();

	// 4. 새 씬 생성 (Factory 호출)
	m_activeScene = it->second();
	if (m_activeScene)
	{
		m_activeScene->Start();
		
		int32 width, height;
		RENDER.UpdateViewport(&width, &height);
		OnScreenResize(width, height);

		SPDLOG_INFO("Successfully loaded scene: {}", scene);
	}
	else
	{
		SPDLOG_ERROR("Failed to create scene instance: {}", scene);
	}
}

Scene* SceneManager::GetActiveScene() const
{
	return m_activeScene.get();
}

void SceneManager::OnScreenResize(int32 width, int32 height)
{
	if (m_activeScene)
		m_activeScene->OnScreenResize(width, height);
}

void SceneManager::Clear()
{
	m_activeScene.reset();
	m_scenes.clear();
}