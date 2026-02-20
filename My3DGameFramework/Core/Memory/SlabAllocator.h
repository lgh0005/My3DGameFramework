#pragma once

namespace MGF3D
{
	CLASS_PTR(SlabMemoryPool)

	template <typename T>
	class SlabAllocator
	{
	public:
		// STL 호환을 위한 필수 타입 정의
		using value_type = T;
		using pointer = T*;
		using size_type = usize;

		// 다른 타입의 할당자로 변환하기 위한 구조체
		template <typename U>
		struct rebind { using other = SlabAllocator<U>; };

	public:
		// 기본 생성자 및 풀 지정 생성자
		SlabAllocator() noexcept;
		SlabAllocator(SlabMemoryPool* pool) noexcept;

		// 복사 생성자
		template <typename U>
		SlabAllocator(const SlabAllocator<U>& other) noexcept;

		// 메모리 할당
		[[nodiscard]] T* allocate(usize n);

		// 메모리 해제
		void deallocate(T* p, usize n) noexcept;

		// 할당자 비교 연산자
		bool operator==(const SlabAllocator& other) const noexcept { return m_pool == other.m_pool; }
		bool operator!=(const SlabAllocator& other) const noexcept { return m_pool != other.m_pool; }
	
	public:
		SlabMemoryPool* m_pool;
	};
}

#include "Memory/SlabAllocator.inl"