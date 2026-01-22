#pragma once
#include "Graphics/RenderPipeline.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(CullingPass)
CLASS_PTR(ShadowPass)
CLASS_PTR(SkyboxPass)
CLASS_PTR(SSAOPass)
CLASS_PTR(PostProcessPass)
CLASS_PTR(UniversalGeometryPass)
CLASS_PTR(UniversalDeferredLightingPass)
CLASS_PTR(UniversalGlobalUniforms)
CLASS_PTR(JoltDebugGizmoPass)
#pragma endregion

CLASS_PTR(UniversalRenderPipeline)
class UniversalRenderPipeline : public RenderPipeline
{
public:
	virtual ~UniversalRenderPipeline();
	static UniversalRenderPipelineUPtr Create();
	virtual void Render(Scene* scene) override;
	virtual void OnResize(int32 width, int32 height) override;

private:
	UniversalRenderPipeline();
	bool Init();

	// 공통 필수 렌더 패스
	CullingPassUPtr		         m_cullingPass      { nullptr };
	ShadowPassUPtr				 m_shadowPass		{ nullptr };
	SkyboxPassUPtr               m_skyboxPass       { nullptr };
	SSAOPassUPtr				 m_ssaoPass			{ nullptr };
	PostProcessPassUPtr			 m_postProcessPass{ nullptr };

	/*OutlinePassUPtr			     m_outlinePass		{ nullptr };
	MotionBlurPassUPtr			 m_motionBlurPass	{ nullptr };*/

	// URP 필수 렌더 패스
	// UniversalPostProcessPassUPtr	   m_postProcessPass	{ nullptr };
	UniversalGeometryPassUPtr		   m_geometryPass       { nullptr };
	UniversalDeferredLightingPassUPtr  m_deferredLightPass  { nullptr };

	// URP 공통 유니폼 변수 버퍼
	UniversalGlobalUniformsUPtr  m_globalUniforms	{ nullptr }; 

	// 디버그
	JoltDebugGizmoPassUPtr		 m_debugGizmoPass   { nullptr };

/*================================//
//   IMGUI debug context method   //
//================================*/
private:
	void RenderIMGUIContext();
};