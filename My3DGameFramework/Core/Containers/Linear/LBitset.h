#pragma once
#include "Containers/TBitset.h"
#include "Memory/LinearAllocator.h"

namespace MGF3D
{
	using LBitset = TBitset<LinearAllocator<bool>>;
}