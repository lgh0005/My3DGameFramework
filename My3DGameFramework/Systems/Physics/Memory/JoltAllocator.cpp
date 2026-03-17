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

	void* JoltAllocator::Reallocate(void* inBlock, usize inOldSize, usize inNewSize)
	{
		// 1. 새로운 크기가 0이면 해제
		if (inNewSize == 0)
		{
			Free(inBlock);
			return nullptr;
		}

		// 2. 기존 블록이 없으면 신규 할당
		if (inBlock == nullptr)
			return Allocate(inNewSize);

		// 3. 새로운 크기의 메모리를 할당
		void* newPtr = Allocate(inNewSize);
		if (newPtr)
		{
			// 4. 데이터 복사
			usize copySize = CommonUtils::Select(inOldSize < inNewSize, inOldSize, inNewSize);
			if (copySize > 0)
				MemoryUtils::Memcpy(newPtr, inBlock, copySize);

			// 5. 기존 메모리 블록을 해제합니다.
			Free(inBlock);
		}

		return newPtr;
	}
}