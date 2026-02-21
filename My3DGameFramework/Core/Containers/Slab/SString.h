#pragma once
#include "Containers/TString.h"
#include "Memory/SlabAllocator.h"

namespace MGF3D
{
	using SString = TString<SlabAllocator<char8>>;
}