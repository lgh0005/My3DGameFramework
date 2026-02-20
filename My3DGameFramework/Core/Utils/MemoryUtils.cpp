#include "CorePch.h"
#include "MemoryUtils.h"

namespace MGF3D
{
	void* MemoryUtils::AlignUp(void* ptr, usize alignment) noexcept
	{
		return reinterpret_cast<void*>(AlignUp(reinterpret_cast<uintptr>(ptr), alignment));
	}
}
