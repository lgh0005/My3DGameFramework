#pragma once
#include "Graphics/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
CLASS_PTR(Program)
CLASS_PTR(ScreenMesh)
CLASS_PTR(PostProcessFramebuffer)
#pragma endregion

CLASS_PTR(MotionBlurPass)
class MotionBlurPass : public ContextRenderPass
{
public:
	~MotionBlurPass();
	static MotionBlurPassUPtr Create(int32 width = WINDOW_WIDTH, int32 heigh = WINDOW_HEIGHT);
	virtual void Render(RenderContext* context) override;
	void Resize(int32 width, int32 height);

private:
	MotionBlurPass();
	bool Init(int32 width, int32 height);

	ScreenMeshPtr					m_plane;
	ProgramPtr						m_motionBlurProgram;
	PostProcessFramebufferUPtr	    m_blurFBO;
};