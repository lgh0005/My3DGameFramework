#pragma once
#include "Containers/TString.h"
#include "Memory/LinearAllocator.h"

namespace MGF3D
{
	using LString = TString<LinearAllocator<char8>>;
}