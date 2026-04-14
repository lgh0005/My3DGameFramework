#pragma once
#include "Managers/TypeManager.h"
#include "Managers/EntityManager.h"
#include "Identities/MGFTypeTree.h"

namespace MGF3D
{
	template<typename T>
	inline void SceneManager::RegisterScene(StringView name)
	{
		// 1. 컴파일 타임 타입 검증
		MGF_STATIC_ASSERT
		(
			std::is_base_of_v<Scene, T>,
			"SceneManager::RegisterScene - 'T' must be derived from 'Scene'."
		);

		MGFTypeTree* tree = MGF_TYPE.GetTree("Scene");
		if (tree) T::s_typeIndex = tree->Register(name, "Scene");
		else
		{
			MGF_LOG_ERROR("SceneManager: Failed to find 'Scene' TypeTree.");
			return;
		}

		int16 typeIndex = T::s_typeIndex;
		if (m_scenes.find(typeIndex) != m_scenes.end())
		{
			MGF_LOG_WARN
			(
				"SceneManager: Scene '{0}' (Index: {1}) is already registered.",
				name, typeIndex
			);
		}

		m_scenes[typeIndex] = SceneFactory([]() -> SceneUPtr { return MakeUnique<T>(); });

		MGF_LOG_INFO
		(
			"SceneManager: Registered Scene '{0}' with TypeIndex {1}.", 
			name, typeIndex
		);
	}

	template<typename T>
	void SceneManager::LoadScene()
	{
		MGF_STATIC_ASSERT
		(
			std::is_base_of_v<Scene, T>,
			"T must derived from 'Scene'."
		);

		int16 typeIndex = T::s_typeIndex;

		// 1. 로드할 씬 탐색
		auto it = m_scenes.find(typeIndex);
		if (it == m_scenes.end())
		{
			MGF_LOG_ERROR("SceneManager: Failed to find scene with TypeIndex '{0}'.", typeIndex);
			return;
		}

		MGF_LOG_INFO("SceneManager: Loading Scene with TypeIndex {0} ...", typeIndex);

		// 2. 이전 씬 파괴
		m_activeScene.reset();
		MGF_ENTITY.Clear();

		// 3. 다음 씬 로드
		m_activeScene = it->second();
		if (!m_activeScene) return;

		if (!m_activeScene->Init())
		{
			MGF_LOG_ERROR("SceneManager: Failed to Initialize Scene TypeIndex '{0}'.", typeIndex);
			m_activeScene.reset();
		}
	}
}