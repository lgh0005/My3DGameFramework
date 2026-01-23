#include "EnginePch.h"
#include "LensFlareEffect.h"

DECLARE_DEFAULTS_IMPL(LensFlareEffect)

LensFlareEffectUPtr LensFlareEffect::Create(int32 priority, int32 width, int32 height)
{
	auto effects = LensFlareEffectUPtr(new LensFlareEffect());
	if (!effects->Init(priority, width, height)) return nullptr;
	return std::move(effects);
}

bool LensFlareEffect::Init(int32 priority, int32 width, int32 height)
{
	m_priority = priority;
	m_width = width;
	m_height = height;

	// TODO

	return true;
}

bool LensFlareEffect::Render(RenderContext* context, Framebuffer* srcFBO, Framebuffer* dstFBO, ScreenMesh* screenMesh)
{
	return false;
}

void LensFlareEffect::OnResize(int32 width, int32 height)
{
}
