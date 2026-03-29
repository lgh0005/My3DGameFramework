#include "CorePch.h"
#include "PoolAlloc.h"
#include "Managers/MemoryManager.h"

namespace MGF3D
{
	void* PoolAlloc::operator new(usize size) noexcept
	{
		return MGF_MEMORY.Allocate(size);
	}

	void* PoolAlloc::operator new[](usize size) noexcept
	{
		return MGF_MEMORY.Allocate(size);
	}

	void PoolAlloc::operator delete(void* ptr, usize size) noexcept
	{
		MGF_MEMORY.Deallocate(ptr, size);
	}

	void PoolAlloc::operator delete[](void* ptr, usize size) noexcept
	{
		MGF_MEMORY.Deallocate(ptr, size);
	}
}