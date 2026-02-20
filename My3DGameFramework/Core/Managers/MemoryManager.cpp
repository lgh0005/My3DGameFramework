#include "CorePch.h"
#include "Managers/MemoryManager.h"
#include "Memory/SlabMemoryPool.h"
#include "Memory/LinearMemoryPool.h"

namespace MGF3D
{
	MemoryManager::MemoryManager()
	{
		/* 힙 영역 메모리 풀 */
		// 1. 전체 필요한 메모리 계산 (4MB * 9개 버킷 = 36MB)
		const usize totalRequirement = INITIAL_POOL_SIZE * SLAB_BUCKET_COUNT;

		// 2. OS로부터 거대한 전역 버퍼를 단 한 번만 할당
		m_baseBuffer = static_cast<byte*>(::operator new(totalRequirement, MGF3D::alignment(DEFAULT_ALIGNMENT)));
	
		// 3. 버퍼를 쪼개서 각 풀에 할당
		usize currentSlotSize = 16;
		byte* currentBufferPtr = m_baseBuffer;
		for (int32 i = 0; i < SLAB_BUCKET_COUNT; ++i)
		{
			// 각 HeapMemoryPool은 이제 nullptr이 아닌, 준비된 전역 버퍼의 일부를 받습니다.
			m_pools[i] = new SlabMemoryPool(currentBufferPtr, INITIAL_POOL_SIZE, currentSlotSize);

			// 다음 버킷을 위해 포인터 이동
			currentBufferPtr += INITIAL_POOL_SIZE;
			currentSlotSize <<= 1;
		}

		/* 스택 영역 메모리 풀 */
		// 1. 프레임 전용 임시 메모리로 32MB 확보
		const usize frameRequirement = 32 * 1024 * 1024;
		m_frameBuffer = static_cast<byte*>(::operator new(frameRequirement, MGF3D::alignment(DEFAULT_ALIGNMENT)));

		// 확보한 힙 버퍼를 Stack 전략으로 관리하는 객체 생성
		m_framePool = new LinearMemoryPool(m_frameBuffer, frameRequirement);
	}

	MemoryManager::~MemoryManager()
	{
		// 1. Stack 영역 해제
		if (m_framePool)
		{
			delete m_framePool;
			m_framePool = nullptr;
		}

		if (m_frameBuffer)
		{
			::operator delete(m_frameBuffer, MGF3D::alignment(DEFAULT_ALIGNMENT));
			m_frameBuffer = nullptr;
		}

		// 2. Slab 영역 해제
		// 풀 객체들 먼저 파괴
		for (int32 i = 0; i < SLAB_BUCKET_COUNT; ++i)
			delete m_pools[i];

		// 마지막으로 전역 버퍼 해제
		if (m_baseBuffer)
		{
			::operator delete(m_baseBuffer, MGF3D::alignment(DEFAULT_ALIGNMENT));
			m_baseBuffer = nullptr;
		}
	}

	void* MemoryManager::Allocate(usize size) noexcept
	{
		// 1. 관리 범위를 벗어나는 거대 할당은 시스템 힙으로 폴백
		if (size > SLAB_MAX_SIZE)
			return ::operator new(size, MGF3D::alignment(MGF3D::DEFAULT_ALIGNMENT));

		// 2. 적절한 버킷 인덱스 찾기
		int32 index = GetPoolIndex(size);
		return m_pools[index]->Allocate();
	}

	void MemoryManager::Deallocate(void* ptr, usize size) noexcept
	{
		if (ptr == nullptr) return;

		// 1. 관리 범위를 벗어나는 거대 할당 해제
		if (size > SLAB_MAX_SIZE)
		{
			::operator delete(ptr, MGF3D::alignment(MGF3D::DEFAULT_ALIGNMENT));
			return;
		}

		// 2. 해당 버킷에 반환
		int32 index = GetPoolIndex(size);
		m_pools[index]->Deallocate(ptr);
	}

	int32 MemoryManager::GetPoolIndex(usize size) const noexcept
	{
		// 16바이트 이하는 무조건 0번 인덱스
		if (size <= 16) return 0;

		// 2의 거듭제곱으로 인덱스 찾기
		usize currentSize = 16;
		for (int i = 0; i < SLAB_BUCKET_COUNT; ++i)
		{
			if (size <= currentSize) return i;
			currentSize <<= 1;
		}

		return SLAB_BUCKET_COUNT - 1;
	}
}