#include "CorePch.h"
#include "SlabMemoryPool.h"
#include "Utils/MemoryUtils.h"

namespace MGF3D
{
	SlabMemoryPool::SlabMemoryPool
	(
		void* buffer, usize slabSize,
		usize slotSize, usize alignment
	) noexcept
		: m_freeList(nullptr), m_slabList(nullptr)
		, m_freeSlotCount(0), m_totalSlotCount(0)
		, m_alignment(alignment) , m_slabSize(slabSize)
	{
		// 1. 슬롯 크기 결정 (최소 포인터 크기 확보 및 정렬)
		m_slotSize = MemoryUtils::AlignUp(Math::Max(slotSize, sizeof(Slot)), alignment);

		// 2. Slab 하나당 들어갈 슬롯 개수 계산
		m_slotsPerSlab = m_slabSize / m_slotSize;

		// 3. 최초 버퍼가 있다면 Slab으로 등록 및 초기화
		if (buffer != nullptr && m_slabSize >= m_slotSize)
		{
			RegisterSlab(buffer, true);
			InitSlotsInSlab(buffer);
		}
	}

	SlabMemoryPool::~SlabMemoryPool()
	{
		Slab* current = m_slabList;
		while (current != nullptr)
		{
			Slab* next = current->next;
			if (current->isExternal == false) ::operator delete(current->address, MGF3D::alignment(m_alignment));
			::operator delete(current);
			current = next;
		}
	}

	void* SlabMemoryPool::Allocate() noexcept
	{
		// 사용 가능한 블록이 없으면 확장 시도
		if (m_freeList == nullptr)
		{
			if (!Expand()) 
				return nullptr;
		}

		// Free List의 머리(Head)를 추출하여 반환
		return PopFreeSlot();
	}
	
	void SlabMemoryPool::Deallocate(void* ptr) noexcept
	{
		if (ptr == nullptr) return;
		PushFreeSlot(ptr);
	}

	usize SlabMemoryPool::GetFreeSlotCount() const noexcept
	{
		return m_freeSlotCount;
	}

	usize SlabMemoryPool::GetUsedSlotCount() const noexcept
	{
		return m_totalSlotCount - m_freeSlotCount;
	}

	usize SlabMemoryPool::GetTotalSlotCount() const noexcept
	{
		return m_totalSlotCount;
	}

	usize SlabMemoryPool::GetSlotSize() const noexcept
	{
		return m_slotSize;
	}

	usize SlabMemoryPool::GetUsedMemory() const noexcept
	{
		// (전체 슬롯 - 남은 슬롯) * 슬롯 크기 = 실제 객체들이 점유 중인 메모리
		return GetUsedSlotCount() * m_slotSize;
	}
	
	/*====================================//
	//    Private Helpers (Slab Logic)    //
	//====================================*/
	void SlabMemoryPool::PushFreeSlot(void* ptr) noexcept
	{
		Slot* slot = static_cast<Slot*>(ptr);
		slot->next = m_freeList;
		m_freeList = slot;
		m_freeSlotCount++;
	}

	void* SlabMemoryPool::PopFreeSlot() noexcept
	{
		Slot* target = m_freeList;
		m_freeList = target->next;
		m_freeSlotCount--;
		return static_cast<void*>(target);
	}

	void SlabMemoryPool::RegisterSlab(void* buffer, bool isExternal) noexcept
	{
		Slab* newSlab = static_cast<Slab*>(::operator new(sizeof(Slab)));
		newSlab->address = buffer;
		newSlab->isExternal = isExternal;
		newSlab->next = m_slabList;
		m_slabList = newSlab;
	}

	void SlabMemoryPool::InitSlotsInSlab(void* buffer) noexcept
	{
		byte* current = static_cast<byte*>(buffer);
		for (usize i = 0; i < m_slotsPerSlab; ++i)
		{
			PushFreeSlot(current);
			current += m_slotSize;
		}
		m_totalSlotCount += m_slotsPerSlab;
	}

	bool SlabMemoryPool::Expand() noexcept
	{
		// 새로운 Slab 메모리 할당
		void* newRawMemory = ::operator new(m_slabSize, MGF3D::alignment(m_alignment));
		if (newRawMemory == nullptr) return false;

		RegisterSlab(newRawMemory, false);
		InitSlotsInSlab(newRawMemory);

		return true;
	}
}