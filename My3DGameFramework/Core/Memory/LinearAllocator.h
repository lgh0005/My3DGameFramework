#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(LinearMemoryPool)

	template <typename T>
	class LinearAllocator
	{
	public:
		using value_type = T;
		Ptr<LinearMemoryPool> m_pool;

		// 생성자: 관리할 스택 풀의 주소를 받습니다.
		LinearAllocator(Ptr<LinearMemoryPool> pool = nullptr) noexcept;

		// 템플릿 복사 생성자 (STL 필수 규격: 다른 타입의 할당자로 변환 가능해야 함)
		template<typename U>
		LinearAllocator(const LinearAllocator<U>& other) noexcept;

		// 메모리 할당과 해제
		[[nodiscard]] Ptr<T> allocate(usize n);
		void deallocate(Ptr<T> p, usize n) noexcept;

		// 할당자 간 비교 연산자 (동일한 풀을 사용하는지 확인)
		bool operator==(const LinearAllocator& other) const noexcept { return m_pool == other.m_pool; }
		bool operator!=(const LinearAllocator& other) const noexcept { return m_pool != other.m_pool; }
	};
}

#include "Memory/LinearAllocator.inl"