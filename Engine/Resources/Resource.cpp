#include "EnginePch.h"
#include "Resource.h"

Resource::Resource() = default;
Resource::~Resource() = default;

bool Resource::MatchesType(ResourceType type) const
{
	return GetResourceType() == type;
}
