#pragma once

namespace MGF3D
{
	template<typename T>
	inline Ptr<T> Chunk<T>::Allocate()
	{
		return m_allocator.allocate(1);
	}

	template<typename T>
	inline void Chunk<T>::Deallocate(Ptr<T> ptr)
	{
		m_allocator.deallocate(ptr, 1);
	}
}