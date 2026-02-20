#pragma once
#include "StackMemoryPool.h"

namespace MGF3D
{
	template <typename T>
	class StackAllocator
	{
	public:
		using value_type = T;
		StackMemoryPool* m_pool;

		// 생성자: 관리할 스택 풀의 주소를 받습니다.
		StackAllocator(StackMemoryPool* pool = nullptr) noexcept;

		// 템플릿 복사 생성자 (STL 필수 규격: 다른 타입의 할당자로 변환 가능해야 함)
		template <typename U>
		StackAllocator(const StackAllocator<U>& other) noexcept;

		// 메모리 할당과 해제
		[[nodiscard]] T* allocate(usize n);
		void deallocate(T* p, usize n) noexcept;

		// 할당자 간 비교 연산자 (동일한 풀을 사용하는지 확인)
		bool operator==(const StackAllocator& other) const noexcept { return m_pool == other.m_pool; }
		bool operator!=(const StackAllocator& other) const noexcept { return m_pool != other.m_pool; }
	};
}

#include "Memory/StackAllocator.inl"