#include "MGFPackedArray.h"
#pragma once

namespace MGF3D
{
	template<typename T> inline MGFPackedArray<T>::MGFPackedArray() = default;
	template<typename T> inline MGFPackedArray<T>::~MGFPackedArray() = default;

	template<typename T>
	template<typename ...Args>
	inline T& MGFPackedArray<T>::Emplace(int32 id, Args && ...args)
	{
		if (Has(id))
		{
			MGF_LOG_WARN("MGFPackedArray: Entity {0} already has this component/object.", id);
			return m_data[m_entityToIndex[id]];
		}

		int32 newIndex = static_cast<int32>(m_data.size());
		m_entityToIndex[id] = newIndex;
		m_indexToEntity[newIndex] = id;

		// 메모리 풀의 맨 끝에 객체를 직접 생성하여 반환
		return m_data.emplace_back(std::forward<Args>(args)...);
	}

	template<typename T>
	inline void MGFPackedArray<T>::Clear()
	{
		m_data.clear();
		m_entityToIndex.clear();
		m_indexToEntity.clear();
	}

	template<typename T>
	inline void MGFPackedArray<T>::MoveToFrontGroup(int32 id)
	{
		auto it = m_entityToIndex.find(id);
		if (it == m_entityToIndex.end()) return;
		
		int32 currentIndex = it->second;

		// 이미 Front 그룹에 있으면 무시
		if (currentIndex < m_boundaryIndex) return;

		// 현재 위치(currentIndex)와 Front 그룹 바로 다음 빈자리(m_boundaryIndex)를 스왑
		if (currentIndex != m_boundaryIndex)
		{
			int32 targetIndex = m_boundaryIndex;
			int32 entityAtTarget = m_indexToEntity[targetIndex];

			std::swap(m_data[currentIndex], m_data[targetIndex]);

			// 매핑 갱신
			m_entityToIndex[id] = targetIndex;
			m_indexToEntity[targetIndex] = id;

			m_entityToIndex[entityAtTarget] = currentIndex;
			m_indexToEntity[currentIndex] = entityAtTarget;
		}

		m_boundaryIndex++;
	}

	template<typename T>
	inline void MGFPackedArray<T>::MoveToBackGroup(int32 id)
	{
		auto it = m_entityToIndex.find(id);
		if (it == m_entityToIndex.end()) return;

		int32 currentIndex = it->second;

		// 이미 Back 그룹에 있으면 무시
		if (currentIndex >= m_boundaryIndex) return;

		// Front 그룹의 맨 마지막 녀석(m_boundaryIndex - 1)과 스왑
		m_boundaryIndex--;

		if (currentIndex != m_boundaryIndex)
		{
			int32 targetIndex = m_boundaryIndex;
			int32 entityAtTarget = m_indexToEntity[targetIndex];

			std::swap(m_data[currentIndex], m_data[targetIndex]);

			m_entityToIndex[id] = targetIndex;
			m_indexToEntity[targetIndex] = id;

			m_entityToIndex[entityAtTarget] = currentIndex;
			m_indexToEntity[currentIndex] = entityAtTarget;
		}
	}

	template<typename T>
	inline void MGFPackedArray<T>::Remove(int32 id)
	{
		auto it = m_entityToIndex.find(id);
		if (it == m_entityToIndex.end()) return;

		// 1. 만약 삭제할 녀석이 Front 그룹에 있다면, 먼저 Back 그룹으로 보냅니다.
		// 이렇게 하면 m_boundaryIndex가 안전하게 감소하고, 삭제할 대상은 Back 그룹 구역으로 이동합니다.
		if (it->second < m_boundaryIndex) MoveToBackGroup(id);

		// 2. 이제 기존의 Swap-and-Pop 로직 수행 (배열의 맨 끝과 스왑 후 제거)
		int32 indexOfRemoved = m_entityToIndex[id];
		int32 indexOfLast = static_cast<int32>(m_data.size()) - 1;

		if (indexOfRemoved < indexOfLast)
		{
			m_data[indexOfRemoved] = std::move(m_data[indexOfLast]);
			int32 entityOfLast = m_indexToEntity[indexOfLast];
			m_entityToIndex[entityOfLast] = indexOfRemoved;
			m_indexToEntity[indexOfRemoved] = entityOfLast;
		}

		m_entityToIndex.erase(id);
		m_indexToEntity.erase(indexOfLast);
		m_data.pop_back();
	}

	template<typename T>
	inline T* MGFPackedArray<T>::Get(int32 id)
	{
		auto it = m_entityToIndex.find(id);
		if (it != m_entityToIndex.end()) return &m_data[it->second];
		return nullptr;
	}

	template<typename T>
	inline bool MGFPackedArray<T>::Has(int32 id) const
	{
		return m_entityToIndex.find(id) != m_entityToIndex.end();
	}
}