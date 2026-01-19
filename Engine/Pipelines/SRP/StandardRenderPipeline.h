#pragma once
#include "Graphics/RenderPipeline.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(CullingPass)
CLASS_PTR(ShadowPass)
CLASS_PTR(SkyboxPass)
CLASS_PTR(JoltDebugGizmoPass)
CLASS_PTR(StandardDeferredLightingPass)
CLASS_PTR(StandardGeometryPass)
CLASS_PTR(StandardSSAOPass)
CLASS_PTR(StandardOutlinePass)
CLASS_PTR(StandardPostProcessPass)
CLASS_PTR(StandardGlobalUniforms)
CLASS_PTR(StandardMotionBlurPass)
CLASS_PTR(GBufferFramebuffer)
CLASS_PTR(PostProcessFramebuffer)
#pragma endregion

CLASS_PTR(StandardRenderPipeline)
class StandardRenderPipeline : public RenderPipeline
{
public:
	virtual ~StandardRenderPipeline();
	static StandardRenderPipelineUPtr Create();
	virtual void Render(Scene* scene)			     override;
	virtual void OnResize(int32 width, int32 height) override;

private:
	StandardRenderPipeline();
	bool Init();

	// 공통 필수 렌더 패스
	CullingPassUPtr					  m_cullingPass		  { nullptr };
	ShadowPassUPtr					  m_shadowPass		  { nullptr };
	SkyboxPassUPtr                    m_skyboxPass		  { nullptr };
	JoltDebugGizmoPassUPtr			  m_debugGizmoPass	  { nullptr };

	// SRP 필수 렌더 패스
	StandardPostProcessPassUPtr		  m_postProcessPass	  { nullptr };
	StandardGeometryPassUPtr		  m_geometryPass	  { nullptr };
	StandardDeferredLightingPassUPtr  m_deferredLightPass { nullptr };
	StandardSSAOPassUPtr			  m_ssaoPass		  { nullptr };
	StandardOutlinePassUPtr		      m_outlinePass		  { nullptr };
	StandardMotionBlurPassUPtr		  m_motionBlurPass	  { nullptr };

	// SRP 공통 유니폼 변수 버퍼
	StandardGlobalUniformsUPtr        m_globalUniforms	  { nullptr };

/*================================//
//   IMGUI debug context method   //
//================================*/
private:
	void RenderIMGUIContext();
};