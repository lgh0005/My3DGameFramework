#pragma once
#include "Graphics/Renderer.h"
#include "Graphics/RenderPipeline.h"

class RenderManager
{
	DECLARE_SINGLE(RenderManager)
	DECLARE_NONINSTANTIABLE(RenderManager)
	using PipelineFactory = std::function<RenderPipelineUPtr()>;

public:
	bool Init();
	void Render(Scene* scene);
	void OnResize(int32 width, int32 height);

	template<typename T>
	void RegisterPipeline(const std::string & name);

	void SetPipeline(const std::string& name);
	Renderer* GetRenderer() const { return m_renderer.get(); }
	void UpdateViewport(int32* outWidth, int32* outHeight);

	void Clear();

private:
	std::unordered_map<std::string, PipelineFactory> m_pipelines;
	RendererUPtr m_renderer;
};

#include "Managers/RenderManager.inl"