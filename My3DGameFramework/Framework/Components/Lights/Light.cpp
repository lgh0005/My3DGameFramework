#include "EnginePch.h"
#include "Light.h"

DECLARE_DEFAULTS_IMPL(Light)

bool Light::MatchesType(ComponentType type) const
{
	return type == ComponentType::Light || Super::MatchesType(type);
}