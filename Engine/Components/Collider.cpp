#include "EnginePch.h"
#include "Collider.h"

DECLARE_DEFAULTS_IMPL(Collider)

bool Collider::MatchesType(ComponentType type) const
{
	if (type == ComponentType::Collider) return true;
	return Super::MatchesType(type);
}
