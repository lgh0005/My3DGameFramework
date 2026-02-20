#pragma once

namespace MGF3D
{
	class StackMemoryPool
	{
		DISABLE_COPY(StackMemoryPool)

	public:
		StackMemoryPool(void* buffer, usize size) noexcept;
		~StackMemoryPool();

	public:
		void* Allocate(usize size, usize alignment = 16) noexcept;
		void Reset() noexcept;

	public:
		usize GetUsedMemory() const noexcept;
		usize GetRemainingMemory() const noexcept;
		bool IsInPool(void* ptr) const noexcept;

	private:
		uintptr m_startAddress;  // 풀의 시작 주소
		usize m_size;			 // 전체 크기
		usize m_offset;			 // 현재까지 사용된 크기
	};
}
