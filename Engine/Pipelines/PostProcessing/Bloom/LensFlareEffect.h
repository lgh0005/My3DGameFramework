#pragma once
#include "Pipelines/PostProcessing/PostProcessEffect.h"

CLASS_PTR(LensFlareEffect)
class LensFlareEffect : public PostProcessEffect
{
public:
	virtual ~LensFlareEffect();
	static LensFlareEffectUPtr Create
	(
		int32 priority,
		int32 width = WINDOW_WIDTH,
		int32 height = WINDOW_HEIGHT
	);

	virtual bool Render
	(
		RenderContext* context,
		Framebuffer* srcFBO,
		Framebuffer* dstFBO,
		ScreenMesh* screenMesh
	) override;

	virtual void OnResize(int32 width, int32 height) override;

private:
	LensFlareEffect();
	bool Init(int32 priority, int32 width, int32 height);

	// DOF (Depth of field)

};