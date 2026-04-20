#include "FrameworkPch.h"
#include "RenderManager.h"
#include "Managers/WindowManager.h"
#include "Rendering/RenderPipeline.h"

namespace MGF3D
{
	RenderManager::RenderManager() = default;
	RenderManager::~RenderManager() = default;

	void RenderManager::Render()
	{
		if (m_activePipeline) m_activePipeline->Render();
	}

	void RenderManager::Resize()
	{
		m_activePipeline->Resize();
	}

	void RenderManager::Shutdown()
	{
		m_activePipeline.reset();
		m_pipelines.clear();
	}

	RenderPipeline* RenderManager::GetActiveRenderPipeline() const
	{
		return m_activePipeline.get();
	}

	void RenderManager::SetRenderPipeline(StringView name)
	{
		auto it = m_pipelines.find(StringHash(name));
		if (it != m_pipelines.end())
		{
			m_activePipeline = it->second();
			Resize();
		}
	}
}