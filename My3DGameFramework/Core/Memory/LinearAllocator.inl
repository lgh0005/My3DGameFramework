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
		else m_pool = MemoryManager::Instance().GetFramePool();
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
		void* ptr = nullptr;

		// 1. 스택 풀 시도
		if (m_pool) ptr = m_pool->Allocate(size, MemoryUtils::GetAlignment(alignof(T)));

		// 2. 스택 실패 시 힙 할당
		if (!ptr) ptr = ::operator new(size);

		return static_cast<T*>(ptr);
	}

	// 메모리 해제
	template <typename T>
	void LinearAllocator<T>::deallocate(T* p, usize n) noexcept
	{
		if (!p) return;

		// 주소가 우리 풀 영역이 아니라면 힙에서 온 녀석입니다.
		if (m_pool)
		{
			if (!m_pool->IsInPool(p)) ::operator delete(p);
		}
		else
		{
			::operator delete(p);
		}
	}
}