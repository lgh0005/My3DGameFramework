#include "EnginePch.h"
#include "Resource.h"

bool Resource::MatchesType(ResourceType type) const
{
	return GetResourceType() == type;
}
