#pragma once
#include "Managers/MemoryManager.h"
#include "Utils/MemoryUtils.h"

namespace MGF3D
{
	// 생성자
	template <typename T>
	LinearAllocator<T>::LinearAllocator(LinearMemoryPool* pool) noexcept
	{
		if (pool) m_pool = pool;
		else m_pool = MemoryManager::Instance().GetLinearMemoryPool();
	}

	// 템플릿 복사 생성자
	template <typename T>
	template <typename U>
	LinearAllocator<T>::LinearAllocator(const LinearAllocator<U>& other) noexcept : m_pool(other.m_pool) {}

	// 메모리 할당
	template <typename T>
	T* LinearAllocator<T>::allocate(usize n)
	{
		const usize size = n * sizeof(T);
		const usize alignmentValue = alignof(T);
		void* ptr = nullptr;

		// 1. 스택 풀 시도
		if (m_pool)
			ptr = m_pool->Allocate(size, MemoryUtils::GetAlignment(alignmentValue));

		// 2. 스택 실패 시 힙 할당
		if (!ptr)
			ptr = ::operator new(size, MGF3D::alignment(alignmentValue));

		return static_cast<T*>(ptr);
	}

	// 메모리 해제
	template <typename T>
	void LinearAllocator<T>::deallocate(T* p, usize n) noexcept
	{
		if (!p) return;
		const usize size = n * sizeof(T);
		const usize alignmentValue = alignof(T);

		// 주소가 우리 풀 영역이 아니라면 힙에서 온 녀석입니다.
		if (m_pool)
		{
			// 풀 외부에 있는 메모리(시스템 힙 할당분)만 명시적으로 해제
			if (!m_pool->IsInPool(p))
				::operator delete(p, size, MGF3D::alignment(alignmentValue));
		}
		else
		{
			// 풀 자체가 없는 상태에서 온 할당분도 Sized Deallocation 적용
			::operator delete(p, size, MGF3D::alignment(alignmentValue));
		}
	}
}