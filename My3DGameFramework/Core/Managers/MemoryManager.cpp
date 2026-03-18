#include "CorePch.h"
#include <new>
#include "Managers/MemoryManager.h"
#include "Utils/MemoryUtils.h"
#include "Memory/SlabMemoryPool.h"
#include "Memory/LinearMemoryPool.h"

namespace MGF3D
{
	MemoryManager::MemoryManager()
		: m_slabMemoryPoolBuffer(nullptr)
		, m_linearMemoryPoolBuffer(nullptr)
		, m_linearMemoryPool(nullptr)
	{
		for (int32 i = 0; i < SlabBucketCount; ++i)
			m_slabMemoryPools[i] = nullptr;
	}

	MemoryManager::~MemoryManager() = default;

	bool MemoryManager::Init()
	{
		// 1. 이중 초기화 방지
		if (m_slabMemoryPoolBuffer != nullptr || m_linearMemoryPoolBuffer != nullptr)
		{
			MGF_LOG_WARN("MemoryManager: Already initialized.");
			return true;
		}

		/*=============================//
		//   힙 영역 메모리 풀 (Slab)   //
		//=============================*/
		// 1. 전체 필요한 메모리 계산 (4MB * 9개 버킷 = 36MB)
		const usize totalRequirement = InitialPoolSize * SlabBucketCount;

		// 2. OS로부터 거대한 전역 버퍼를 단 한 번만 할당 (std::nothrow를 사용하여 실패 시 nullptr 반환)
		m_slabMemoryPoolBuffer = static_cast<byte*>(::operator new(totalRequirement, MGF3D::alignment(DefaultAlignment), std::nothrow));
		if (!m_slabMemoryPoolBuffer)
		{
			MGF_LOG_FATAL("MemoryManager: Failed to allocate global slab buffer! (Out of memory)");
			return false;
		}

		// 3. 버퍼를 쪼개서 각 풀에 할당
		usize currentSlotSize = 16;
		byte* currentBufferPtr = m_slabMemoryPoolBuffer;
		for (int32 i = 0; i < SlabBucketCount; ++i)
		{
			// 각 HeapMemoryPool은 이제 nullptr이 아닌, 준비된 전역 버퍼의 일부를 받습니다.
			m_slabMemoryPools[i] = new SlabMemoryPool(currentBufferPtr, InitialPoolSize, currentSlotSize);

			// 다음 버킷을 위해 포인터 이동
			currentBufferPtr += InitialPoolSize;
			currentSlotSize <<= 1;
		}

		/*=================================//
		//   스택 영역 메모리 풀 (Linear)   //
		//=================================*/
		const usize frameRequirement = 32 * 1024 * 1024;
		m_linearMemoryPoolBuffer = static_cast<byte*>(::operator new(frameRequirement, MGF3D::alignment(DefaultAlignment), std::nothrow));
		if (!m_linearMemoryPoolBuffer)
		{
			MGF_LOG_FATAL("MemoryManager: Failed to allocate global linear buffer! (Out of memory)");
			Shutdown();
			return false;
		}

		// 확보한 힙 버퍼를 Stack 전략으로 관리하는 객체 생성
		m_linearMemoryPool = new LinearMemoryPool(m_linearMemoryPoolBuffer, frameRequirement);

		return true;
	}

	void MemoryManager::Shutdown()
	{
		// 1. Linear 메모리 영역 해제
		if (m_linearMemoryPool)
		{
			delete m_linearMemoryPool;
			m_linearMemoryPool = nullptr;
		}

		if (m_linearMemoryPoolBuffer)
		{
			::operator delete(m_linearMemoryPoolBuffer, MGF3D::alignment(DefaultAlignment));
			m_linearMemoryPoolBuffer = nullptr;
		}

		// 2. Slab 메모리 영역 해제
		// 2-1. 풀 객체들 먼저 파괴
		for (int32 i = 0; i < SlabBucketCount; ++i)
		{
			if (m_slabMemoryPools[i])
			{
				delete m_slabMemoryPools[i];
				m_slabMemoryPools[i] = nullptr;
			}
		}

		// 2-2. 마지막으로 전역 버퍼 해제
		if (m_slabMemoryPoolBuffer)
		{
			::operator delete(m_slabMemoryPoolBuffer, MGF3D::alignment(DefaultAlignment));
			m_slabMemoryPoolBuffer = nullptr;
		}
	}

	void* MemoryManager::Allocate(usize size) noexcept
	{
		// 1. 관리 범위를 벗어나는 거대 할당은 시스템 힙으로 폴백
		if (size > SlabMaxSize)
			return ::operator new(size, MGF3D::alignment(MGF3D::DefaultAlignment));

		// 2. 적절한 버킷 인덱스 찾기
		int32 index = GetPoolIndex(size);
		return m_slabMemoryPools[index]->Allocate();
	}

	void MemoryManager::Deallocate(void* ptr, usize size) noexcept
	{
		if (ptr == nullptr) return;

		// 1. 관리 범위를 벗어나는 거대 할당 해제
		if (size > SlabMaxSize)
		{
			::operator delete(ptr, MGF3D::alignment(MGF3D::DefaultAlignment));
			return;
		}

		// 2. 해당 버킷에 반환
		int32 index = GetPoolIndex(size);
		m_slabMemoryPools[index]->Deallocate(ptr);
	}

	Ptr<SlabMemoryPool> MemoryManager::GetSlabMemoryPool(usize size) const noexcept
	{
		// 1. 관리 가능한 최대 슬랩 크기(4096)를 넘어가면 풀이 없으므로 nullptr 반환
		if (size > SlabMaxSize || size == 0)
			return nullptr;

		// 2. 내부 인덱스 계산 로직 활용
		int32 index = GetPoolIndex(size);

		// 3. 인덱스 유효성 검사 후 해당 풀 포인터 반환
		if (CommonUtils::IsInRange<usize>(index, 0, SlabBucketCount))
			return m_slabMemoryPools[index];

		return nullptr;
	}

	Ptr<LinearMemoryPool> MemoryManager::GetLinearMemoryPool() const noexcept
	{
		return m_linearMemoryPool;
	}

	int32 MemoryManager::GetPoolIndex(usize size) const noexcept
	{
		// 1. 16바이트 이하는 무조건 0번 인덱스
		if (size <= 16) return 0;

		// 2. 비트 연산을 이용한 O(1) 인덱스 계산
		uint32 msb = MemoryUtils::MostSignificantBit64(static_cast<uint64>(size - 1));
		int32 index = static_cast<int32>(msb) - 3;

		// 3. 안전을 위한 클램핑
		if (index < 0) return 0;
		if (index >= SlabBucketCount) return SlabBucketCount - 1;

		return index;
	}

	usize MemoryManager::GetTotalSlabUsedMemory() const noexcept
	{
		usize total = 0;
		for (int32 i = 0; i < SlabBucketCount; ++i)
		{
			if (m_slabMemoryPools[i])
				total += m_slabMemoryPools[i]->GetUsedMemory();
		}
		return total;
	}
}