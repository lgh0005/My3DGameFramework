#pragma once
#include "Textures/GLTexture.h"

namespace MGF3D
{
	template<typename T> inline TexturePool<T>::TexturePool(uint32 maxCapacity)
		: m_totalCount(0), m_maxCapacity(maxCapacity) { }

	template<typename T>
	inline TexturePool<T>::~TexturePool()
	{
		Clear();
	}

	template<typename T>
	inline void TexturePool<T>::Release(SharedPtr<T> texture)
	{
		if (texture == nullptr) return;

		// MRU(가장 최근 자원)를 위해 뒤로 삽입
		usize textureHash = texture->GetHash().value;
		m_available[textureHash].PushBack(texture);
		m_totalCount++;

		// 임계값 초과 시 LRU(가장 오래된 자원) 제거
		if (m_totalCount > m_maxCapacity) Trim();
	}

	template<typename T>
	inline void TexturePool<T>::Clear()
	{
		for (auto& available : m_available) available.second.Clear();
		m_available.Clear();
		m_totalCount = 0;
	}

	template<typename T>
	inline SharedPtr<T> TexturePool<T>::FindAvailable(usize textureHash)
	{
		auto pList = m_available.Find(textureHash);
		if (pList && !pList->Empty())
		{
			auto texture = pList->PopBack();
			m_totalCount--;
			if (pList->Empty()) m_available.Remove(textureHash);
			return texture;
		}
		return nullptr;
	}

	template<typename T>
	inline void TexturePool<T>::Trim()
	{
		for (auto& available : m_available)
		{
			if (!available.second.Empty())
			{
				available.second.PopFront();
				m_totalCount--;

				if (available.second.Empty())
					m_available.Remove(available.first);

				return;
			}
		}
	}
}