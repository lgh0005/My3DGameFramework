#include "CorePch.h"
#include "MemoryStreamBuffer.h"
#include "MemoryStreamBufferPool.h"

namespace MGF3D
{
	MemoryStreamBuffer::MemoryStreamBuffer(usize capacity)
	{
		m_data.Resize(capacity);
	}

	MemoryStreamBuffer::~MemoryStreamBuffer() = default;
}