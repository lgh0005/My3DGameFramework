#pragma once
#include "Graphics/Renderer.h"
#include "Graphics/RenderPipeline.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Renderer)
CLASS_PTR(RenderPipeline)
#pragma endregion

class RenderManager
{
	DECLARE_SINGLE(RenderManager)
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

/*====================================//
//   RenderManager template inlines   //
//====================================*/
template<typename T>
inline void RenderManager::RegisterPipeline(const std::string& name)
{
	m_pipelines[name] = []() -> RenderPipelineUPtr
	{
		return T::Create();
	};
}