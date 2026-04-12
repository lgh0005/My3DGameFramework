#pragma once
#include "Managers/SceneManager.h"

namespace MGF3D
{
	class Scenes
	{
		MGF_DECLARE_UTILITIES(Scenes)

	public:

		// 1. 씬 등록 API
		template<typename T> static void Register(StringView name);
		
		// 2. 씬 로드 API
		template<typename T> static void Load();

		// 3. 현재 활성화된 씬 반환 API
		static class Scene* GetActiveScene();
	};
}

#include "Interfaces/Scenes.inl"