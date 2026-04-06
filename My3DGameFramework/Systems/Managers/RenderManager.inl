#pragma once

template<typename T>
inline void RenderManager::RegisterPipeline(const std::string& name)
{
	m_pipelines[name] = []() -> RenderPipelineUPtr
	{
		return T::Create();
	};
}