#pragma once
#include "Managers/TypeManager.h"

namespace MGF3D
{
	template<typename T>
	inline void RenderManager::RegisterRenderPipeline(StringView name)
	{
		// 1. 컴파일 타임 타입 검증
		MGF_STATIC_ASSERT
		(
			std::is_base_of_v<RenderPipeline, T>,
			"RenderManager::RegisterRenderPipeline - 'T' must be derived from 'RenderPipeline'."
		);

		// 2. 이름 해싱 (int64 키 생성)
		int64 hash = StringHash(name);

		// 2. 팩토리 함수 등록
		m_pipelines[hash] = []() -> RenderPipelineUPtr
		{
			return T::Create();
		};
	}
}
