#pragma once
#include "Containers/TBitset.h"
#include "Memory/SlabAllocator.h"

namespace MGF3D
{
	using SBitset = TBitset<SlabAllocator<bool>>;
}