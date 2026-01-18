#pragma once
#include "Graphics/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
CLASS_PTR(Program)
CLASS_PTR(ScreenMesh)
CLASS_PTR(PostProcessFramebuffer)
#pragma endregion

CLASS_PTR(StandardMotionBlurPass)
class StandardMotionBlurPass : public ContextRenderPass
{
public:
	~StandardMotionBlurPass();
	static StandardMotionBlurPassUPtr Create
	(
		int32 width = WINDOW_WIDTH,
		int32 heigh = WINDOW_HEIGHT
	);
	virtual void Render(RenderContext* context) override;
	void Resize(int32 width, int32 height);

private:
	StandardMotionBlurPass();
	bool Init(int32 width, int32 height);

	ScreenMeshPtr					m_plane;
	ProgramPtr						m_motionBlurProgram;
	PostProcessFramebufferUPtr	    m_blurFBO;
};