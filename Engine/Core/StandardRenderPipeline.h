#pragma once
#include "RenderPipeline.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(StandardDeferredLightingPass)
CLASS_PTR(StandardGeometryPass)
CLASS_PTR(StandardPostProcessPass)
CLASS_PTR(StandardShadowPass)
CLASS_PTR(StandardSkyboxPass)
CLASS_PTR(StandardGlobalUniforms)
CLASS_PTR(CubeTexture)
#pragma endregion

CLASS_PTR(StandardRenderPipeline)
class StandardRenderPipeline : public RenderPipeline
{
public:
	static StandardRenderPipelineUPtr Create();
	virtual void Render(Scene* scene)			     override;
	virtual void OnResize(int32 width, int32 height) override;

/*==================================//
//   standard render pass getters   //
//==================================*/
public:
	StandardShadowPass* GetShadowPass()        const { return m_shadowPass.get(); }
	StandardSkyboxPass* GetSkyboxPass()        const { return m_skyboxPass.get(); }
	StandardPostProcessPass* GetPostProcessPass()   const { return m_postProcessPass.get(); }
	StandardGeometryPass* GetGeometryPass()      const { return m_geometryPass.get(); }
	StandardDeferredLightingPass* GetDeferredLightPass() const { return m_deferredLightPass.get(); }
	StandardGlobalUniforms* GetGlobalUniforms()    const { return m_globalUniforms.get(); }

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
	StandardShadowPassUPtr		      m_shadowPass		  { nullptr };
	StandardSkyboxPassUPtr			  m_skyboxPass		  { nullptr };
	StandardPostProcessPassUPtr		  m_postProcessPass	  { nullptr };
	StandardGeometryPassUPtr		  m_geometryPass	  { nullptr };
	StandardDeferredLightingPassUPtr  m_deferredLightPass { nullptr };
	StandardGlobalUniformsUPtr        m_globalUniforms	  { nullptr };

/*================================//
//   IMGUI debug context method   //
//================================*/
private:
	void RenderIMGUIContext();
};