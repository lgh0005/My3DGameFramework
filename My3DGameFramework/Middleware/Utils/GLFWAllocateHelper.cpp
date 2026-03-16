#include "MiddlewarePch.h"
#include "GLFWAllocateHelper.h"
#include "Managers/MemoryManager.h"
#include "Utils/MemoryUtils.h"

namespace MGF3D
{
	void* GLFWAllocateHelper::GLFW_Allocate(usize size, void*)
	{
		// 1. 헤더 크기만큼 더 할당
		usize totalSize = MemoryUtils::GetRequiredSizeWithHeader(size, DefaultAlignment);

		// 2. MemoryManager에서 할당 (4096 이하면 Slab, 넘으면 System)
		void* ptr = MGF_MEMORY.Allocate(totalSize);
		if (!ptr) return nullptr;

		// 3. 헤더 패킹 및 사용자가 쓸 완벽하게 정렬된 주소 반환
		return MemoryUtils::PackHeader(ptr, totalSize, DefaultAlignment);
	}

	void GLFWAllocateHelper::GLFW_Deallocate(void* block, void* user)
	{
		if (!block) return;

		// 1. 숨겨진 엔진 표준 헤더 언패킹
		MemoryHeader* header = MemoryUtils::UnpackHeader(block);

		// 2. 헤더에 보존된 원본 주소(rawPtr)와 원본 크기(size)로 완벽한 해제
		MGF_MEMORY.Deallocate(header->rawPtr, header->size);
	}

	void* GLFWAllocateHelper::GLFW_Reallocate(void* block, usize size, void* user)
	{
		// 1. 블록이 없으면 신규 할당, 사이즈가 0이면 해제
		if (!block) return GLFW_Allocate(size, user);
		if (size == 0) 
		{
			GLFW_Deallocate(block, user);
			return nullptr;
		}

		// 2. 메모리 재할당: 새로운 크기만큼 신규 블록을 먼저 할당
		void* newBlock = GLFW_Allocate(size, user);
		if (newBlock) 
		{
			// 3. 기존 블록의 헤더에서 정보 획득
			// 기존 데이터의 순수 크기 역산 : (전체 크기 - 헤더 크기 - 정렬 패딩)
			Ptr<MemoryHeader> oldHeader = MemoryUtils::UnpackHeader(block);
			usize oldDataSize = oldHeader->size - sizeof(MemoryHeader) - DefaultAlignment;

			// 4. 기존 데이터와 새 데이터 크기 중 작은 쪽을 선택하여 안전하게 복사
			usize copySize = CommonUtils::Select
			(
				(oldDataSize < size),
				oldDataSize,
				size
			);

			MemoryUtils::Memcpy(newBlock, block, copySize);

			// 5. 복사가 끝났으니 기존 블록은 제거
			GLFW_Deallocate(block, user);
		}
		return newBlock;
	}
}