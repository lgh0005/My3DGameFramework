#pragma once

namespace MGF3D
{
	class SlabMemoryPool;
	class LinearMemoryPool;

	class MemoryManager
	{
		MGF_DECLARE_SINGLE(MemoryManager)

	private:
		MemoryManager();
		~MemoryManager();

	public:
		bool Init();
		void Shutdown();

	public:
		void* Allocate(usize size) noexcept;
		void  Deallocate(void* ptr, usize size) noexcept;

		Ptr<SlabMemoryPool> GetSlabMemoryPool(usize size) const noexcept;
		Ptr<LinearMemoryPool> GetLinearMemoryPool() const noexcept;
		usize GetTotalSlabUsedMemory() const noexcept;

		void* AllocateLinearChunk(usize size) noexcept;

	private:
		// Slab 기반 메모리 풀 영역 멤버들
		int32 GetPoolIndex(usize size) const noexcept; // 크기에 따른 풀의 인덱스를 계산합니다
		byte* m_slabMemoryPoolBuffer; // 1. 전역 힙 버퍼의 시작 주소
		Ptr<SlabMemoryPool> m_slabMemoryPools[MGF3D::SlabBucketCount]; // 2. 관리 중인 버킷(Slab)들

		// Linear 기반 메모리 풀 영역 멤버들
		byte* m_linearMemoryPoolBuffer;
		Ptr<LinearMemoryPool> m_linearMemoryPool;

		// 워커 스레드들이 가질 자신만의 메모리 풀 영역 멤버들 (Linear 기반)
		byte* m_threadLocalMemoryPoolBuffer;
		Ptr<LinearMemoryPool> m_threadLocalMemoryPool;
		Mutex m_threadLocalAllocationMutex;
	};
}
