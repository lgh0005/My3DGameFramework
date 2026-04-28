#pragma once

namespace MGF3D
{
	template<typename T, usize ChunkSize>
	class MGFChunk
	{
	public:
		MGFChunk();
		~MGFChunk();

	public:
		template<typename... Args> T* Allocate(usize index, Args&&... args);
		void Free(usize index);
		T* GetElement(usize index);

	private:
		T* m_buffer;
		Array<bool, ChunkSize> m_allocated;
	};
}

#include "ECS/MGFChunk.inl"