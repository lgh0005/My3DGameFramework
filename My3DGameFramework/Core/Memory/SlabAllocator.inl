#pragma once
#include "Managers/MemoryManager.h"
#include "Memory/SlabMemoryPool.h"

namespace MGF3D
{
	template <typename T>
	SlabAllocator<T>::SlabAllocator() noexcept : m_pool(nullptr) { }

	template <typename T>
	SlabAllocator<T>::SlabAllocator(SlabMemoryPool* pool) noexcept : m_pool(pool) { }

	template <typename T>
	template <typename U>
	SlabAllocator<T>::SlabAllocator(const SlabAllocator<U>& other) noexcept : m_pool(other.m_pool) { }

	template <typename T>
	[[nodiscard]] T* SlabAllocator<T>::allocate(usize n)
	{
		if (n == 0) return nullptr;

		// 1. 요청된 전체 크기 계산
		const usize totalSize = n * sizeof(T);

		// 2. 만약 특정 풀(m_pool)이 지정되어 있고, 단일 객체(n=1) 할당이라면 해당 풀을 우선 사용
		if (n == 1 && m_pool != nullptr)
		{
			if (sizeof(T) <= m_pool->GetSlotSize())
				return static_cast<T*>(m_pool->Allocate());
		}

		// 3. 그 외의 모든 경우(n > 1 배열 포함)는 전역 MemoryManager의 슬랩 시스템에 위임
		// 매니저는 totalSize를 보고 최적의 버킷(16~4096)을 골라주거나 시스템 힙으로 폴백합니다.
		return static_cast<T*>(MemoryManager::Instance().Allocate(totalSize));
	}

	template <typename T>
	void SlabAllocator<T>::deallocate(T* p, usize n) noexcept
	{
		if (p == nullptr) return;

		const usize totalSize = n * sizeof(T);

		// 1. 할당 시와 동일하게 특정 풀에서 가져온 것인지 확인
		if (n == 1 && m_pool != nullptr)
		{
			if (sizeof(T) <= m_pool->GetSlotSize())
			{
				m_pool->Deallocate(p);
				return;
			}
		}

		// 2. 나머지는 전역 MemoryManager를 통해 반환
		MemoryManager::Instance().Deallocate(p, totalSize);
	}
}