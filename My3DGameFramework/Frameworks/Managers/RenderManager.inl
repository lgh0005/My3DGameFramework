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

		// TODO : 타입 트리 추가 필요

		m_pipelines[name] = RenderPipelineFactory([]() -> RenderPipelineUPtr { return MakeUnique<T>(); });
	}
}

//template<typename T>
//inline void RenderManager::RegisterPipeline(const std::string& name)
//{
//	m_pipelines[name] = []() -> RenderPipelineUPtr
//	{
//		return T::Create();
//	};
//}