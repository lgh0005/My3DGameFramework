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
	inline void MGFPackedArray<T>::RemoveOnEntityDestroyed(int32 id)
	{
		auto it = m_entityToIndex.find(id); // 오타 수정
		if (it == m_entityToIndex.end()) return;

		int32 indexOfRemoved = it->second;
		int32 indexOfLast = static_cast<int32>(m_data.size()) - 1;

		// 삭제될 원소가 맨 끝이 아니라면, 맨 끝 원소를 끌어와서 덮어씀
		if (indexOfRemoved < indexOfLast)
		{
			// std::move를 통해 복사 비용 없이 내부 포인터만 스와핑
			m_data[indexOfRemoved] = std::move(m_data[indexOfLast]);

			// 끌어온 녀석의 맵 인덱스 갱신
			int32 entityOfLast = m_indexToEntity[indexOfLast];
			m_entityToIndex[entityOfLast] = indexOfRemoved;
			m_indexToEntity[indexOfRemoved] = entityOfLast;
		}

		// 맵에서 지우고 배열 길이 1 감소 (O(1) 비용)
		m_entityToIndex.erase(id);
		m_indexToEntity.erase(indexOfLast);
		m_data.pop_back();
	}

	template<typename T>
	inline void MGFPackedArray<T>::Clear()
	{
		m_data.clear();
		m_entityToIndex.clear();
		m_indexToEntity.clear();
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