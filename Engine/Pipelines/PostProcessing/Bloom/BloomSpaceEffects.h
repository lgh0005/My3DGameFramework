#pragma once
#include "Pipelines/PostProcessing/PostProcessEffect.h"

CLASS_PTR(BloomSpaceEffects)
class BloomSpaceEffects : public PostProcessEffect
{
public:
	virtual ~BloomSpaceEffects();
	static BloomSpaceEffectsUPtr Create
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
	BloomSpaceEffects();
	bool Init(int32 priority, int32 width, int32 height);
};