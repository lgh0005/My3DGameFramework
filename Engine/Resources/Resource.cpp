#include "EnginePch.h"
#include "Resource.h"

DECLARE_DEFAULTS_IMPL(Resource)

bool Resource::MatchesType(ResourceType type) const
{
	return GetResourceType() == type;
}
