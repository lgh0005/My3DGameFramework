#include "CorePch.h"
#include "MemoryStreamBufferPool.h"
#include "MemoryStreamBuffer.h"

namespace MGF3D
{
	MemoryStreamBufferPool::MemoryStreamBufferPool() = default;
	MemoryStreamBufferPool::~MemoryStreamBufferPool()
	{
		MGF_LOCK_SCOPE(m_mutex);
		m_buckets.Clear();
	}

	MemoryStreamBufferPtr MemoryStreamBufferPool::Acquire(usize requiredSize)
	{
		// 1. 사이즈 규격화 (예: 1024, 2048, 4096...)
		usize bucketSize = 1024;
		while (bucketSize < requiredSize) bucketSize <<= 1;

		{
			MGF_LOCK_SCOPE(m_mutex);

			// 2. 해당 버킷에 여유 버퍼가 있는지 확인
			auto it = m_buckets.Find(bucketSize);
			if (it && !it->Empty())
			{
				auto buffer = it->Back();
				it->PopBack();
				return buffer;
			}
		}

		// 3. 풀에 없다면 새로 생성
		return MakeShared<MemoryStreamBuffer>(bucketSize);
	}

	void MemoryStreamBufferPool::Release(MemoryStreamBufferPtr buffer)
	{
		if (!buffer) return;

		MGF_LOCK_SCOPE(m_mutex);

		// 버퍼의 실제 용량을 키로 사용하여 해당 버킷 바구니에 다시 넣습니다.
		usize capacity = buffer->GetCapacity();
		m_buckets[capacity].PushBack(Move(buffer));
	}
}