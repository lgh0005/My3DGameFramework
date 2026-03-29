#include "CorePch.h"
#include "MemoryStreamBufferPool.h"
#include "MemoryStreamBuffer.h"
#include "Utils/MemoryUtils.h"

namespace MGF3D
{
	MemoryStreamBufferPool::MemoryStreamBufferPool() = default;
	MemoryStreamBufferPool::~MemoryStreamBufferPool()
	{
		MGF_LOCK_SCOPE(m_mutex);
		for (int i = 0; i < 64; ++i)
			m_buckets[i].Clear();
	}

	MemoryStreamBufferPtr MemoryStreamBufferPool::Acquire(usize requiredSize)
	{
		// 1. 최소 사이즈 보장 및 2의 거듭제곱 규격화
		usize bucketSize = 1024;
		while (bucketSize < requiredSize) bucketSize <<= 1;

		// 2. 비트 연산을 통한 O(1) 인덱스 도출
		uint32 index = MemoryUtils::MostSignificantBit64(static_cast<uint64>(bucketSize));

		{
			MGF_LOCK_SCOPE(m_mutex);

			// 3. 배열 인덱스로 즉시 접근
			if (!m_buckets[index].Empty())
			{
				auto buffer = m_buckets[index].Back();
				m_buckets[index].PopBack();
				return buffer;
			}
		}

		// 3. 풀에 없다면 새로 생성
		return MakeShared<MemoryStreamBuffer>(bucketSize);
	}

	void MemoryStreamBufferPool::Release(MemoryStreamBufferPtr buffer)
	{
		if (!buffer) return;

		usize capacity = buffer->GetCapacity();
		uint32 index = MemoryUtils::MostSignificantBit64(static_cast<uint64>(capacity));

		MGF_LOCK_SCOPE(m_mutex);
		m_buckets[index].PushBack(Move(buffer));
	}
}