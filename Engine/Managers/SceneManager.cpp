#include "EnginePch.h"
#include "SceneManager.h"
#include "Scene/Scene.h"

/*===================================//
//   default scene manager methods   //
//===================================*/
void SceneManager::LoadScene(const std::string& scene, const std::string& pipeline)
{
	// 1. 등록된 씬을 먼저 찾기
	auto it = m_scenes.find(scene);
	if (it == m_scenes.end())
	{
		LOG_ERROR("Failed to find scene registration for name: {}", scene);
		return;
	}

	// 2. 렌더 파이프라인 교체 (셰이더, 렌더 타겟 등 재설정)
	RENDER.SetPipeline(pipeline);

	// 3. 기존 씬 정리 (소멸자 호출 -> 리소스 해제)
	UI.Clear();
	m_activeScene.reset();
	OBJECT.Clear();
	LOG_INFO("Loading Scene: {} ...", scene);

	// 4. 새 씬 메모리 할당 (Factory 호출)
	m_activeScene = it->second();
	if (m_activeScene)
	{
		// [Step 1] Init: 리소스 로드 및 환경 설정
		// 실패하면 씬 로딩 자체를 중단해야 함
		if (!m_activeScene->Init())
		{
			LOG_ERROR("Failed to Initialize Scene: {}", scene);
			m_activeScene.reset();
			return;
		}

		// [Step 2] Awake: 게임 오브젝트 배치 (OnPlaceActors)
		// 이 함수 내부에서 ProcessPendingAdds()가 호출되어 카메라 등이 실제 생성됨
		m_activeScene->Awake();

		// [Step 3] Resize: 카메라 뷰포트 갱신
		// Awake에서 카메라가 생성되었으므로, 이제 뷰포트를 맞춰줘야 함
		int32 width, height;
		RENDER.UpdateViewport(&width, &height);
		OnScreenResize(width, height);

		// [Step 4] Start: 게임 로직 시작 (OnBeginPlay)
		// 모든 객체가 준비된 상태에서 상호작용 시작
		m_activeScene->Start();

		LOG_INFO("Successfully loaded scene: {}", scene);
	}
	else
	{
		LOG_ERROR("Failed to create scene instance: {}", scene);
	}
}

Scene* SceneManager::GetActiveScene() const
{
	return m_activeScene.get();
}

void SceneManager::OnScreenResize(int32 width, int32 height)
{
	if (m_activeScene) m_activeScene->OnScreenResize(width, height);
}

void SceneManager::Clear()
{
	m_activeScene.reset();
	m_scenes.clear();
	LOG_INFO("SceneManager Cleared.");
}

/*=========================================//
//   active scene state checking methods   //
//=========================================*/