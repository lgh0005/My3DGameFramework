#include "PhysicsPch.h"
#include "JoltAllocator.h"
#include "Managers/MemoryManager.h"
#include "Utils/MemoryUtils.h"

namespace MGF3D
{
	void* JoltAllocator::Allocate(usize inSize)
	{
		return MGF_MEMORY.Allocate(inSize);
	}

	void JoltAllocator::Free(void* inBlock)
	{
		if (inBlock == nullptr) return;
		// 주의: 현재 MemoryManager::Deallocate는 size를 요구합니다.
		// 만약 Slab 풀에서 블록 크기를 스스로 알지 못한다면, 
		// 할당 시 size를 헤더에 기록해두는 등 Jolt의 Free(void*) 서명에 맞출 래퍼 로직이 필요할 수 있습니다.
		// 임시 방편으로 전역 delete를 쓰거나, MemoryManager를 확장해야 합니다.
		// 여기서는 예시로 MemoryManager에 크기를 모르는 해제 API가 있다고 가정하거나, 아래 Aligned 방식처럼 헤더를 씁니다.

		// *임시 구현*: Jolt는 기본 Allocate/Free를 거의 안 씁니다 (대부분 Aligned 사용)
		::operator delete(inBlock);
	}

	void* JoltAllocator::AlignedAllocate(usize inSize, usize inAlignment)
	{

	}

	void JoltAllocator::AlignedFree(void* inBlock)
	{

	}
}