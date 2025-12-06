#pragma once
#include "Core/RenderPipeline.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(StandardCullingPass)
CLASS_PTR(StandardDeferredLightingPass)
CLASS_PTR(StandardGeometryPass)
CLASS_PTR(StandardSSAOPass)
CLASS_PTR(StandardPostProcessPass)
CLASS_PTR(StandardShadowPass)
CLASS_PTR(StandardSkyboxPass)
CLASS_PTR(StandardGlobalUniforms)
#pragma endregion

CLASS_PTR(StandardRenderPipeline)
class StandardRenderPipeline : public RenderPipeline
{
public:
	static StandardRenderPipelineUPtr Create();
	virtual void Render(Scene* scene)			     override;
	virtual void OnResize(int32 width, int32 height) override;

private:
	StandardRenderPipeline() = default;
	bool Init();
	void BlitCopyDepth
	(
		Framebuffer* src, Framebuffer* dst,
		int32 width = WINDOW_WIDTH,
		int32 height = WINDOW_HEIGHT
	);

	// 필수 렌더 패스
	StandardCullingPassUPtr		      m_cullingPass		  { nullptr };
	StandardShadowPassUPtr		      m_shadowPass		  { nullptr };
	StandardSkyboxPassUPtr			  m_skyboxPass		  { nullptr };
	StandardPostProcessPassUPtr		  m_postProcessPass	  { nullptr };
	StandardGeometryPassUPtr		  m_geometryPass	  { nullptr };
	StandardDeferredLightingPassUPtr  m_deferredLightPass { nullptr };
	StandardSSAOPassUPtr			  m_ssaoPass		  { nullptr };
	StandardGlobalUniformsUPtr        m_globalUniforms	  { nullptr };

/*================================//
//   IMGUI debug context method   //
//================================*/
private:
	void RenderIMGUIContext();
};