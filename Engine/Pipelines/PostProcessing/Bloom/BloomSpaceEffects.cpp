#include "EnginePch.h"
#include "BloomSpaceEffects.h"

DECLARE_DEFAULTS_IMPL(BloomSpaceEffects)

BloomSpaceEffectsUPtr BloomSpaceEffects::Create(int32 priority, int32 width, int32 height)
{
	auto effects = BloomSpaceEffectsUPtr(new BloomSpaceEffects());
	if (!effects->Init(priority, width, height)) return nullptr;
	return std::move(effects);
}

bool BloomSpaceEffects::Init(int32 priority, int32 width, int32 height)
{
	m_priority = priority;
	m_width = width;
	m_height = height;

	// TODO

	return true;
}

bool BloomSpaceEffects::Render(RenderContext* context, Framebuffer* srcFBO, Framebuffer* dstFBO, ScreenMesh* screenMesh)
{
	return false;
}

void BloomSpaceEffects::OnResize(int32 width, int32 height)
{
}
