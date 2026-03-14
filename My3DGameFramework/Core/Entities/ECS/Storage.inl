#pragma once

namespace MGF3D
{
	template<typename T>
	template<typename ...Args>
	inline T* Storage<T>::Emplace(Args && ...args)
	{
		//T* comp = m_chunk.Allocate();
		//new (comp) T(std::forward<Args>(args)...);

		//usize index = m_denseArray.Size();
		//m_denseArray.PushBack(comp);
		//comp->SetRegistryIndex(index);

		//return comp;
	}

	template<typename T>
	inline void Storage<T>::Remove(usize index)
	{
		//if (index >= m_denseArray.Count()) return;

		//T* targetComp = m_denseArray[index];

		//if (targetComp)
		//{
		//	targetComp->OnDestroy();
		//	targetComp->~T();
		//	m_chunk.Deallocate(targetComp);
		//}

		//// Swap & Pop: 마지막 요소의 인덱스를 현재 삭제되는 위치로 갱신
		//if (index != m_denseArray.Count() - 1)
		//	m_denseArray.Back()->SetRegistryIndex(index);

		//m_denseArray.RemoveSwap(index);
	}

	template<typename T>
	inline void Storage<T>::Defragment()
	{
		//if (!m_isDirty || m_denseArray.Empty()) return;

		//int32 left = 0;
		//int32 right = (int32)m_denseArray.Count() - 1;

		//while (left <= right)
		//{
		//	while (left <= right && m_denseArray[left]->IsActive())
		//		left++;

		//	while (left <= right && !m_denseArray[right]->IsActive())
		//		right--;

		//	if (left < right)
		//	{
		//		CommonUtils::Swap(m_denseArray[left], m_denseArray[right]);
		//		m_denseArray[left]->SetRegistryIndex(left);
		//		m_denseArray[right]->SetRegistryIndex(right);

		//		left++;
		//		right--;
		//	}
		//}

		//// 정렬 결과(활성 개수)를 업데이트하고 플래그 초기화
		//m_activeCount = (usize)left;
		//m_isDirty = false;
	}

	// TODO : 컴포넌트 상태 업데이트 메서드
	template<typename T>
	inline void Storage<T>::Update()
	{
	}

	template<typename T>
	inline void Storage<T>::FixedUpdate()
	{
	}

	template<typename T>
	inline void Storage<T>::LateUpdate()
	{
	}

	template<typename T>
	inline void Storage<T>::Clear()
	{
	}
}