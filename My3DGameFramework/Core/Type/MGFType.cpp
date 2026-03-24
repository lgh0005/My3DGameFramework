#include "CorePch.h"
#include "MGFType.h"

namespace MGF3D
{
	MGFType::MGFType() = default;
	MGFType::~MGFType() = default;

	bool MGFType::IsA(const MGFType* other) const
	{
		if (!other) return false;
		if (other->depth > depth) return false;
		return chain[other->depth] == other->id;
	}
}