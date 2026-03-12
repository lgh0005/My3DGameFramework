#pragma once
#include "Pointer/SmartPtrAliases.h"
#include "Managers/MemoryManager.h"

namespace MGF3D
{
	template<typename T>
	inline void MGFSmartPtrDeleter<T>::operator()(T* ptr) const
	{
		if (ptr)
		{
			ptr->~T();
			MemoryManager::Instance().Deallocate(ptr, sizeof(T));
		}
	}

	template<typename T>
	inline T* MGFSmartPtrAllocator<T>::allocate(std::size_t n)
	{
		return static_cast<T*>(MemoryManager::Instance().Allocate(n * sizeof(T)));
	}

	template<typename T>
	inline void MGFSmartPtrAllocator<T>::deallocate(T* p, std::size_t n)
	{
		if (p) MemoryManager::Instance().Deallocate(p, n * sizeof(T));
	}
}