#pragma once
#include "Memory/SlabAllocator.h"

namespace MGF3D
{
	template<typename T>
	class Chunk
	{
	public:
		Ptr<T> Allocate();
		void Deallocate(Ptr<T> ptr);

	private:
		SlabAllocator<T> m_allocator;
	};
}

#include "Entities/ECS/Chunk.inl"