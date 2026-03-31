#pragma once
#include "Entities/ECS/Chunk.h"
#include "Entities/Entity/Component/ComponentStateMachine.h"

namespace MGF3D
{
	template<typename T>
	template<typename ...Args>
	inline T* Storage<T>::Emplace(Args && ...args)
	{
		// 1. 현재 사용 중인 청크가 없거나 가득 찼다면 새 청크 생성
		if (m_currentChunk == nullptr || m_currentChunk->IsFull())
		{
			// SlabMemoryPool 주소는,
			// SlabAllocator의 기본 로직(MGF_MEMORY)을 타도록 null 전달 가능
			auto newChunk = MakeUnique<Chunk<T>>(nullptr);
			m_currentChunk = newChunk.Get();
			m_chunks.PushBack(Move(newChunk));
		}

		// 2. 청크의 연속된 메모리에서 공간 확보
		T* memory = m_currentChunk->Allocate();

		// 3. Placement New로 객체 생성
		T* comp = new (memory) T(std::forward<Args>(args)...);

		// 4. 관리 리스트(Dense Array) 등록
		usize index = m_denseArray.Count();
		m_denseArray.PushBack(comp);
		comp->SetStorageIndex(index);

		return comp;
	}

	template<typename T>
	inline void Storage<T>::Remove(usize index)
	{
		if (index >= m_denseArray.Count()) return;

		T* targetComp = m_denseArray[index];
		if (targetComp)
		{
			targetComp->OnDestroy();
			targetComp->~T();
		}

		// Swap & Pop: 마지막 요소의 인덱스를 현재 삭제되는 위치로 갱신
		usize lastIndex = m_denseArray.Size() - 1;
		if (index != lastIndex) m_denseArray.Back()->SetStorageIndex(index);
		m_denseArray.RemoveSwap(index);

		// 4. 상태 변경 알림
		// 객체가 사라졌으므로 Defragment가 필요할 수 있음을 알림
		MarkStorageDirty();
	}

	template<typename T>
	inline void Storage<T>::Defragment()
	{
		// m_isStorageDirty 플래그가 켜졌을 때만 수행
		if (!m_isStorageDirty || m_denseArray.Empty()) return;

		int32 left = 0;
		int32 right = (int32)m_denseArray.Size() - 1;

		// 투 포인터 방식의 파티셔닝
		while (left <= right)
		{
			// 왼쪽 포인터: Active인 동안 전진
			while (left <= right && m_denseArray[left]->GetComponentStateMachine()->IsActive())
				left++;

			// 오른쪽 포인터: Active가 아닌 동안 후진
			while (left <= right && m_denseArray[right]->GetComponentStateMachine()->IsActive())
				right--;

			if (left < right)
			{
				// 교체 후 인덱스 동기화
				CommonUtils::Swap(m_denseArray[left], m_denseArray[right]);
				m_denseArray[left]->SetStorageIndex((usize)left);
				m_denseArray[right]->SetStorageIndex((usize)right);
				left++;
				right--;
			}
		}

		// 이제 앞쪽의 '진짜 활성 상태' 개수 확정
		m_activeCount = (usize)left;
		m_isStorageDirty = false;
	}

	template<typename T>
	inline void Storage<T>::Update()
	{
		for (usize i = 0; i < m_activeCount; ++i) m_denseArray[i]->Update();
	}

	template<typename T>
	inline void Storage<T>::FixedUpdate()
	{
		for (usize i = 0; i < m_activeCount; ++i) m_denseArray[i]->FixedUpdate();
	}

	template<typename T>
	inline void Storage<T>::LateUpdate()
	{
		for (usize i = 0; i < m_activeCount; ++i) m_denseArray[i]->LateUpdate();
	}

	template<typename T>
	inline void Storage<T>::Clear()
	{
		// 1. 모든 컴포넌트의 명시적 소멸자 호출 및 메모리 반환
		for (T* comp : m_denseArray)
		{
			if (comp) comp->~T();
		}

		// 2. 컨테이너 및 관리 지표 초기화
		m_denseArray.Clear();
		m_activeCount = 0;
		m_isStorageDirty = false;

		// 3. 청크 전체 파괴 (UniquePtr에 의해 Chunk 소멸자 호출 -> 16KB 뭉텅이 반환)
		m_chunks.Clear();
		m_currentChunk = nullptr;
	}
}