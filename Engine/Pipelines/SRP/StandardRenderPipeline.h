#pragma once
#include "Graphics/RenderPipeline.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(CullingPass)
CLASS_PTR(ShadowPass)
CLASS_PTR(SkyboxPass)
CLASS_PTR(SSAOPass)
CLASS_PTR(OutlinePass)
CLASS_PTR(MotionBlurPass)
CLASS_PTR(StandardDeferredLightingPass)
CLASS_PTR(StandardGeometryPass)
CLASS_PTR(StandardPostProcessPass)
CLASS_PTR(StandardGlobalUniforms)
CLASS_PTR(JoltDebugGizmoPass)
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
	SSAOPassUPtr					  m_ssaoPass		  { nullptr };
	OutlinePassUPtr					  m_outlinePass		  { nullptr };
	MotionBlurPassUPtr				  m_motionBlurPass	  { nullptr };

	// SRP 필수 렌더 패스
	StandardPostProcessPassUPtr		  m_postProcessPass	  { nullptr };
	StandardGeometryPassUPtr		  m_geometryPass	  { nullptr };
	StandardDeferredLightingPassUPtr  m_deferredLightPass { nullptr };

	// SRP 공통 유니폼 변수 버퍼
	StandardGlobalUniformsUPtr        m_globalUniforms	  { nullptr };

	// 디버그
	JoltDebugGizmoPassUPtr			  m_debugGizmoPass{ nullptr };

/*================================//
//   IMGUI debug context method   //
//================================*/
private:
	void RenderIMGUIContext();
};