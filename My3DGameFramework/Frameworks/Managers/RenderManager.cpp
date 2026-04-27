#include "FrameworkPch.h"
#include "RenderManager.h"
#include "Managers/WindowManager.h"
#include "Managers/InstanceManager.h"
#include "Rendering/RenderPipeline.h"
#include "Rendering/RenderContext.h"

namespace MGF3D
{
	RenderManager::RenderManager() = default;
	RenderManager::~RenderManager() = default;

	bool RenderManager::Init()
	{
		m_renderContext = MakeUnique<RenderContext>();
		if (!m_renderContext) return false;
		return true;
	}

	void RenderManager::Render()
	{
		if (!m_activePipeline || !m_renderContext) return;

		// 1. 씬의 엔티티들로부터 데이터 추출 (핵심)
		MGF_INSTANCE.Extract(m_renderContext.get());

		// 2. 파이프라인 실행
		m_activePipeline->Render(m_renderContext.get());

		// 3. 다음 프레임을 위한 큐 정리
		m_renderContext->ClearQueues();
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