#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(RenderPipeline)
	MGF_CLASS_PTR(RenderContext)

	class RenderManager
	{
		MGF_DECLARE_SINGLE(RenderManager)
		using RenderPipelineFactory = Func<RenderPipelineUPtr>;

	private:
		RenderManager();
		~RenderManager();

	public:
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
		RenderContextUPtr m_renderContext;
	};
}

#include "Managers/RenderManager.inl"