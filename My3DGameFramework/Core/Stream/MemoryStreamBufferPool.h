#pragma once
#include "Pointer/PoolAlloc.h"

namespace MGF3D
{
	MGF_CLASS_PTR(MemoryStreamBuffer)

	class MemoryStreamBufferPool : public PoolAlloc
	{
	public:
		MemoryStreamBufferPool();
		virtual ~MemoryStreamBufferPool() override;

	public:
		MemoryStreamBufferPtr Acquire(usize requiredSize);
		void Release(MemoryStreamBufferPtr buffer);

	private:
		Mutex m_mutex;
		SMap<usize, SVector<MemoryStreamBufferPtr>> m_buckets;
	};
}