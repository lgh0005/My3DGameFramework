#pragma once

namespace MGF3D
{
	template<typename T>
	inline Chunk<T>::Chunk(SlabMemoryPool* pool) : m_allocator(pool)
	{
		m_buffer = m_allocator.allocate(MAX_ITEMS);
	}

	template<typename T>
	inline Chunk<T>::~Chunk()
	{
		if (m_buffer)
		{
			m_allocator.deallocate(m_buffer, MAX_ITEMS);
			m_buffer = nullptr;
		}
	}

	template<typename T>
	inline Ptr<T> Chunk<T>::Allocate()
	{
		if (IsFull()) return nullptr;
		return &m_buffer[m_useCount++];
	}
}