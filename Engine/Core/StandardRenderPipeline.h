#pragma once

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
class StandardRenderPipeline
{
public:
	static StandardRenderPipelineUPtr Create();
	void Render(Scene* scene);

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

	// 필수 ubo
	StandardGlobalUniformsUPtr        m_globalUniforms	  { nullptr };
};