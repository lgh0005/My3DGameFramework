#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(RenderPipeline)
	MGF_CLASS_PTR(RenderContext)
	MGF_CLASS_PTR(RenderCollector)

	class RenderManager
	{
		MGF_DECLARE_SINGLE(RenderManager)
		using RenderPipelineFactory = Func<RenderPipelineUPtr>;

	private:
		RenderManager();
		~RenderManager();

	public:
		bool Init();
		void Render();
		void Resize();
		void Shutdown();

	public:
		RenderPipeline* GetActiveRenderPipeline() const;
		template<typename T> void RegisterRenderPipeline(StringView name);
		void SetRenderPipeline(StringView name);

	public:
		HashMap<int64, RenderPipelineFactory> m_pipelines;
		RenderPipelineUPtr m_activePipeline;
		RenderCollectorUPtr m_renderCollector;
		RenderContextUPtr m_renderContext;
	};
}

#include "Managers/RenderManager.inl"