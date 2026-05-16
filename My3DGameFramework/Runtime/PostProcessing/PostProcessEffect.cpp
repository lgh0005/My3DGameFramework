#include "RuntimePch.h"
#include "PostProcessEffect.h"

namespace MGF3D
{ 
	PostProcessEffect::PostProcessEffect() = default;
	PostProcessEffect::~PostProcessEffect() = default;

	bool PostProcessEffect::IsEfectEnabled() const
	{
		return m_isEffectEnable;
	}

	void PostProcessEffect::SetEffectEnabled(bool enabled)
	{
		m_isEffectEnable = enabled;
	}
}