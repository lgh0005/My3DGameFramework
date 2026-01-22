#pragma once
#include "Pipelines/PostProcessing/PostProcessEffect.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
CLASS_PTR(Program)
CLASS_PTR(PostProcessFramebuffer)
#pragma endregion

CLASS_PTR(MotionBlurEffect)
class MotionBlurEffect : public PostProcessEffect
{
	using Super = PostProcessEffect;

public:
	virtual ~MotionBlurEffect() override;
	static MotionBlurEffectUPtr Create
	(
		int32 priority,
		int32 width = WINDOW_WIDTH,
		int32 height = WINDOW_HEIGHT
	);
	virtual bool Render
	(
		RenderContext* context, 
		Framebuffer* mainFBO, 
		ScreenMesh* screenMesh
	) override;
	virtual void OnResize(int32 width, int32 height) override;

private:
	MotionBlurEffect();
	bool Init(int32 priority, int32 width, int32 height);

	ProgramPtr						m_motionBlurProgram;
	PostProcessFramebufferUPtr	    m_motionBlurFBO;
};