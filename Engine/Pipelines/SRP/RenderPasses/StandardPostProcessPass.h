#pragma once
#include "Graphics/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(Program)
CLASS_PTR(Framebuffer)
CLASS_PTR(PostProcessFramebuffer)
CLASS_PTR(ScreenMesh)
CLASS_PTR(RenderContext)
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(StandardPostProcessPass)
class StandardPostProcessPass : public ContextRenderPass
{
public:
	virtual ~StandardPostProcessPass();
	static StandardPostProcessPassUPtr Create
	(
		int32 width = WINDOW_WIDTH,
		int32 heigh = WINDOW_HEIGHT
	);
	PostProcessFramebuffer* GetFramebuffer() const { return m_frameBuffer.get(); }
	virtual void Render(RenderContext* context) override;
	void Resize(int32 width, int32 height);

/*=============================================//
//   standard post-processing helper methods   //
//=============================================*/
private:
	void ExtractBrightAreas(Texture* sceneTexture);
	Texture* ComputeGaussianBlur();
	void RenderCompositePass(Texture* sceneTexture, Texture* bloomTexture);

private:
	StandardPostProcessPass();
	bool Init(int32 width, int32 height);

	ScreenMeshPtr						  m_plane;
	ProgramPtr							  m_blurProgram;
	ProgramPtr							  m_compositeProgram;
	ProgramPtr							  m_thresholdProgram;
	PostProcessFramebufferUPtr			  m_frameBuffer;
	PostProcessFramebufferUPtr			  m_pingPongFBOs[2];
	TexturePtr							  m_cameraDirtTexture;
	float m_gamma						  { 2.2f };
	float m_exposure					  { 1.0f };
};