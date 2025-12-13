#pragma once
#include "Core/RenderPipeline.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(CullingPass)
CLASS_PTR(ShadowPass)
CLASS_PTR(SSAOPass)
CLASS_PTR(SkyboxPass)
CLASS_PTR(StandardSkyboxPass)
CLASS_PTR(StandardDeferredLightingPass)
CLASS_PTR(StandardGeometryPass)
CLASS_PTR(StandardPostProcessPass)
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

	// 공통 필수 렌더 패스
	CullingPassUPtr		      m_cullingPass				  { nullptr };
	ShadowPassUPtr		      m_shadowPass				  { nullptr };
	SSAOPassUPtr			  m_ssaoPass				  { nullptr };
	SkyboxPassUPtr            m_skyboxPass				  { nullptr };

	// SRP 필수 렌더 패스
	StandardPostProcessPassUPtr		  m_postProcessPass	  { nullptr };
	StandardGeometryPassUPtr		  m_geometryPass	  { nullptr };
	StandardDeferredLightingPassUPtr  m_deferredLightPass { nullptr };

	// SRP 공통 유니폼 변수 버퍼
	StandardGlobalUniformsUPtr        m_globalUniforms	  { nullptr };

/*================================//
//   IMGUI debug context method   //
//================================*/
private:
	void RenderIMGUIContext();
};