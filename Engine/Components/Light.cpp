#include "EnginePch.h"
#include "Light.h"

DECLARE_DEFAULTS_IMPL(Light)

bool Light::MatchesType(ComponentType type) const
{
	if (type == ComponentType::Light) return true;
	return Super::MatchesType(type);
}