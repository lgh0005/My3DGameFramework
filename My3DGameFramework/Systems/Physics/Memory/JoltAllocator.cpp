#include "PhysicsPch.h"
#include "JoltAllocator.h"
#include "Managers/MemoryManager.h"
#include "Utils/MemoryUtils.h"

namespace MGF3D
{
	void* JoltAllocator::Allocate(usize inSize)
	{
		return AlignedAllocate(inSize, DefaultAlignment);
	}

	void JoltAllocator::Free(void* inBlock)
	{
		if (!inBlock) return;
		Ptr<MemoryHeader> header = MemoryUtils::UnpackHeader(inBlock);
		MGF_MEMORY.Deallocate(header->rawPtr, header->size);
	}

	void* JoltAllocator::AlignedAllocate(usize inSize, usize inAlignment)
	{
		const usize totalSize = MemoryUtils::GetRequiredSizeWithHeader(inSize, inAlignment);
		void* ptr = MGF_MEMORY.Allocate(totalSize);
		if (!ptr) return nullptr;
		return MemoryUtils::PackHeader(ptr, totalSize, inAlignment);
	}

	void JoltAllocator::AlignedFree(void* inBlock)
	{
		Free(inBlock);
	}
}