#pragma once
#include "Pointer/PoolAlloc.h"

namespace MGF3D
{
	MGF_CLASS_PTR(MemoryStreamBuffer)
	class MemoryStreamBufferPool : public PoolAlloc
	{
	public:
		MemoryStreamBufferPool();
		~MemoryStreamBufferPool();

	public:
		MemoryStreamBufferPtr Acquire(usize requiredSize);
		void Release(MemoryStreamBufferPtr buffer);

	private:
		Mutex m_mutex;
		SVector<MemoryStreamBufferPtr> m_buckets[64];
	};
}