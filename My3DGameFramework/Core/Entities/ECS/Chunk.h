#pragma once
#include "Memory/SlabAllocator.h"

namespace MGF3D
{
	template<typename T>
	class Chunk
	{
		MGF_DISABLE_COPY(Chunk)
		static constexpr usize MAX_ITEMS = CommonUtils::Select
		(
			sizeof(T) > ChunkMemorySize, 
			1,
			ChunkMemorySize / sizeof(T)
		);

	public:
		Chunk(SlabMemoryPool* pool = nullptr);
		~Chunk();

	public:
		Ptr<T> Allocate();
		bool IsFull() const { return m_useCount >= MAX_ITEMS; }
		usize GetCount() const { return m_useCount; }

	private:
		SlabAllocator<T> m_allocator;
		Ptr<T>           m_buffer   { nullptr }; // 뭉텅이 메모리 시작 주소
		usize            m_useCount { 0 };       // 현재 할당된 개수
	};
}

#include "Entities/ECS/Chunk.inl"