#include "PhysicsPch.h"
#include "JoltMemoryPool.h"
#include "Managers/MemoryManager.h"
#include "Utils/MemoryUtils.h"

namespace MGF3D
{
	JoltMemoryPool::JoltMemoryPool(usize inCapacity)
		: m_baseAddress(nullptr)
		, m_capacity(inCapacity)
		, m_offset(0)
	{
		// 임시 할당을 위한 거대한 연속된 메모리 블록을 통째로 빌려옵니다.
		// (SlabMaxSize를 초과하므로 시스템 메모리로 안전하게 잡힐 것입니다)
		m_baseAddress = MGF_MEMORY.Allocate(m_capacity);
		MGF_ASSERT(m_baseAddress != nullptr, "Failed to allocate Jolt TempAllocator buffer!");
	}

	JoltMemoryPool::~JoltMemoryPool()
	{
		// 소멸 시점에 반환되지 않은 메모리(메모리 누수)가 있는지 엄격히 검사
		MGF_ASSERT(m_offset == 0, "JoltMemoryPool destroyed with active allocations!");
		if (m_baseAddress)
		{
			MGF_MEMORY.Deallocate(m_baseAddress, m_capacity);
			m_baseAddress = nullptr;
		}
	}

	void* JoltMemoryPool::Allocate(JPH::uint inSize)
	{
		if (inSize == 0) return nullptr;

		// 1. Jolt의 연산은 SIMD 최적화가 많아 16바이트 정렬을 강력히 권장합니다.
		usize alignedSize = MemoryUtils::AlignUp(static_cast<usize>(inSize), DefaultAlignment);

		// 2. 용량 초과 검사
		if (m_offset + alignedSize > m_capacity)
		{
			MGF_LOG_FATAL("[Physics] Jolt TempAllocator Out of Memory! Capacity: {0}, Req: {1}", m_capacity, alignedSize);
			MGF_ASSERT(false, "Increase JoltMemoryPool capacity!");
			return nullptr;
		}

		// 3. 현재 오프셋 위치의 포인터 계산 (MemoryUtils::PtrAdd 활용)
		void* ptr = MemoryUtils::PtrAdd(m_baseAddress, m_offset);

		// 4. 오프셋 증가 (스택을 쌓아올림)
		m_offset += alignedSize;

		return ptr;
	}

	void JoltMemoryPool::Free(void* inAddress, JPH::uint inSize)
	{
		if (!inAddress || inSize == 0) return;

		usize alignedSize = MemoryUtils::AlignUp(static_cast<usize>(inSize), DefaultAlignment);

		// Jolt의 임시 할당자는 반드시 후입선출(LIFO) 순서로 메모리를 반환
		// 방금 해제하려는 메모리의 주소가 정확히 스택의 최상단 블록인지 검사
		void* expectedAddress = MemoryUtils::PtrAdd(m_baseAddress, m_offset - alignedSize);
		MGF_ASSERT(inAddress == expectedAddress, "Jolt TempAllocator LIFO violation!");

		// 5. 검증을 통과했다면 오프셋만 뒤로 물러납니다. (O(1) 속도의 초고속 해제)
		m_offset -= alignedSize;
	}

	void JoltMemoryPool::Reset()
	{
		MGF_ASSERT(m_offset == 0, "Resetting JoltMemoryPool while it is still in use!");
		m_offset = 0;
	}
}