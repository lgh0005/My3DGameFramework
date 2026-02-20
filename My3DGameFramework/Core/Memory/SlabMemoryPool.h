#pragma once

namespace MGF3D
{
	/**
	 * @brief 특정 크기의 객체들만 관리하는 Slab 방식의 메모리 풀입니다.
	 */
	class SlabMemoryPool
	{
		DISABLE_COPY(SlabMemoryPool)

	private:

		// Slab 내부에서 비어 있는 한 칸(Slot)을 가리키는 구조체
		struct Slot { Slot* next; };

		// 실제 메모리 덩어리인 Slab을 추적하는 구조체
		struct Slab { void* address; Slab* next; bool isExternal; };

	public:
		/**
		 * @param buffer 최초에 주입할 메모리 (nullptr 가능)
		 * @param slabSize 한 Slab(덩어리)의 크기
		 * @param slotSize 관리할 객체 한 칸의 크기
		 * @param alignment 정렬 기준
		 */
		SlabMemoryPool
		(
			void* buffer,
			usize slabSize,
			usize slotSize,
			usize alignment = MGF3D::DEFAULT_ALIGNMENT
		) noexcept;
		~SlabMemoryPool();

		void* Allocate() noexcept;
		void Deallocate(void* ptr) noexcept;
		usize GetFreeSlotCount() const noexcept;
		usize GetSlotSize() const noexcept;

	/*====================================//
	//   SlabMemoryPool private helpers   //
	//====================================*/
	private:
		void  PushFreeSlot(void* ptr) noexcept;
		void* PopFreeSlot() noexcept;
		void  RegisterSlab(void* buffer, bool isExternal) noexcept;
		void  InitSlotsInSlab(void* buffer) noexcept;
		bool Expand() noexcept;

	private:
		Slot* m_freeList;      // 빈 슬롯들의 헤드
		Slab* m_slabList;      // 할당된 Slab(덩어리)들의 목록

		usize m_slotSize;      // 정렬이 반영된 슬롯 한 칸의 크기
		usize m_slabSize;      // Slab 하나의 전체 크기
		usize m_slotsPerSlab;  // Slab 하나당 들어가는 슬롯 개수
		usize m_freeSlotCount; // 현재 사용 가능한 총 슬롯 개수
		usize m_alignment;     // 메모리 정렬 기준
	};
}
