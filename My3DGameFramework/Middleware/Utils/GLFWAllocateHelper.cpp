#include "MiddlewarePch.h"
#include "GLFWAllocateHelper.h"
#include "Managers/MemoryManager.h"
#include <GLFW/glfw3.h>

namespace MGF3D
{
	void* GLFWAllocateHelper::GLFW_Allocate(usize size, void*)
	{
		// 1. 헤더 크기만큼 더 할당
		usize totalSize = size + sizeof(GLFWMemHeader);

		// 2. MemoryManager에서 할당 (4096 이하면 Slab, 넘으면 System)
		void* ptr = MGF_MEMORY.Allocate(totalSize);
		if (!ptr) return nullptr;

		// 3. 앞부분에 크기 기록하고 실제 데이터 시작 주소 반환
		GLFWMemHeader* header = static_cast<GLFWMemHeader*>(ptr);
		header->size = totalSize;

		// 4. 헤더 바로 뒤의 주소(사용자가 쓸 실제 데이터 영역)를 반환
		// 포인터 연산에 의해 header + 1은 sizeof(GLFWMemHeader)만큼 뒤를 가리킵니다.
		return header + 1;
	}

	void GLFWAllocateHelper::GLFW_Deallocate(void* block, void* user)
	{
		if (!block) return;

		// 1. 사용자 데이터 주소에서 헤더 크기만큼 뒤로 이동하여 헤더 위치 탐색
		GLFWMemHeader* header = static_cast<GLFWMemHeader*>(block) - 1;

		// 2. 헤더에 적혀있던 '할당 당시 전체 크기'를 인자로 넘겨 MemoryManager가 버킷을 찾게 함
		// 이로써 GLFW의 메모리도 완벽하게 Slab Pool로 반환되거나 시스템 해제됩니다.
		MGF_MEMORY.Deallocate(header, header->size);
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
			// 2. 기존 블록의 헤더에서 원본 크기 획득
			GLFWMemHeader* oldHeader = static_cast<GLFWMemHeader*>(block) - 1;
			usize oldDataSize = oldHeader->size - sizeof(GLFWMemHeader);

			// 3. 기존 데이터와 새 데이터 크기 중 작은 쪽을 선택하여 안전하게 복사
			usize copySize = CommonUtils::Select
			(
				(oldDataSize < size),
				oldDataSize,
				size
			);

			std::memcpy(newBlock, block, copySize);

			// 4. 복사가 끝났으니 기존 블록은 제거
			GLFW_Deallocate(block, user);
		}
		return newBlock;
	}
}