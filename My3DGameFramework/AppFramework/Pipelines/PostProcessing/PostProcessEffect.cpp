#include "EnginePch.h"
#include "PostProcessEffect.h"

DECLARE_DEFAULTS_IMPL(PostProcessEffect)

void PostProcessEffect::OnResize(int32 width, int32 height)
{
	m_width = width;
	m_height = height;
}
