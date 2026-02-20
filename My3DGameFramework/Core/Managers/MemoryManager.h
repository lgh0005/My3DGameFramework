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

		LinearMemoryPool* GetFramePool() const noexcept { return m_framePool; }

	private:
		// 힙 영역 멤버들
		int32 GetPoolIndex(usize size) const noexcept; // 크기에 따른 풀의 인덱스를 계산합니다
		byte* m_baseBuffer; // 1. 전역 힙 버퍼의 시작 주소
		SlabMemoryPool* m_pools[SLAB_BUCKET_COUNT]; // 2. 관리 중인 버킷(Slab)들

		// 스택 영역 멤버들
		byte* m_frameBuffer;
		LinearMemoryPool* m_framePool;
	};
}
