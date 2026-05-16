#include "RuntimePch.h"
#include "HDREffects.h"
#include "Rendering/RenderContext.h"
#include "Programs/GraphicsProgram.h"

namespace MGF3D
{
	HDREffects::HDREffects() = default;
	HDREffects::~HDREffects() = default;

	HDREffectsUPtr HDREffects::Create()
	{
		return nullptr;
	}

	bool HDREffects::Init()
	{
		return true;
	}

	void HDREffects::Resize(int32 width, int32 height)
	{

	}

	bool HDREffects::Render
	(
		RenderContext* context,
		GLFramebuffer2D* srcFBO,
		GLFramebuffer2D* dstFBO
	)
	{
		return true;
	}
}
