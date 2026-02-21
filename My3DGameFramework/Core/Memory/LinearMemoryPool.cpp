#include "CorePch.h"
#include "LinearMemoryPool.h"
#include "Utils/MemoryUtils.h"

namespace MGF3D
{
	LinearMemoryPool::LinearMemoryPool(void* buffer, usize size) noexcept
		: m_startAddress(reinterpret_cast<uintptr>(buffer)), 
		  m_size(size), m_offset(0) { }

	LinearMemoryPool::~LinearMemoryPool() { }

	void* LinearMemoryPool::Allocate(usize size, usize alignment) noexcept
	{
		// 현재 오프셋에서 정렬 조건을 만족하는 주소를 계산
		uintptr currentAddress = m_startAddress + m_offset;
		uintptr alignedAddress = MemoryUtils::AlignUp(currentAddress, alignment);
		
		// 정렬로 인해 추가된 오프셋을 계산
		usize alignmentPadding = alignedAddress - currentAddress;
		usize totalNeeded = size + alignmentPadding;

		// 남은 공간이 충분한지 확인
		if (m_offset + totalNeeded > m_size) return nullptr;

		// 오프셋을 갱신하고 할당된 주소를 반환
		m_offset += totalNeeded;
		return reinterpret_cast<void*>(alignedAddress);
	}

	void LinearMemoryPool::Reset() noexcept
	{
		m_offset = 0;
	}

	usize LinearMemoryPool::GetUsedMemory() const noexcept
	{
		return m_offset;
	}

	usize LinearMemoryPool::GetRemainingMemory() const noexcept
	{
		return m_size - m_offset; 
	}

	bool LinearMemoryPool::IsInPool(void* ptr) const noexcept
	{
		uintptr addr = reinterpret_cast<uintptr>(ptr);
		return (addr >= m_startAddress && addr < m_startAddress + m_size);
	}
}