#pragma once

namespace MGF3D
{
	CLASS_PTR(SlabMemoryPool)
	CLASS_PTR(LinearMemoryPool)

	class MemoryManager
	{
		DECLARE_SINGLE(MemoryManager)

	private:
		MemoryManager();
		~MemoryManager();

	public:
		void* Allocate(usize size) noexcept;
		void  Deallocate(void* ptr, usize size) noexcept;

		SlabMemoryPool* GetSlabPool(usize size) const noexcept;
		LinearMemoryPool* GetFramePool() const noexcept;

	private:
		// Slab 기반 메모리 풀 영역 멤버들
		int32 GetPoolIndex(usize size) const noexcept; // 크기에 따른 풀의 인덱스를 계산합니다
		byte* m_baseBuffer; // 1. 전역 힙 버퍼의 시작 주소
		SlabMemoryPool* m_pools[MGF3D::SlabBucketCount]; // 2. 관리 중인 버킷(Slab)들

		// Linear 기반 메모리 풀 영역 멤버들
		byte* m_frameBuffer;
		LinearMemoryPool* m_framePool;
	};
}
