#include "CorePch.h"
#include "ThreadLocalPool.h"
#include "Managers/MemoryManager.h"

namespace MGF3D
{
	ThreadLocalPool::TlsWrapper::TlsWrapper() noexcept
		: m_buffer(nullptr), m_pool(nullptr)
	{
		// 스레드당 2MB 할당
		const usize threadRequirement = 2 * 1024 * 1024;
		m_buffer = static_cast<byte*>(MGF_MEMORY.AllocateLinearChunk(threadRequirement));
		if (m_buffer)
		{
			// 떼어온 2MB짜리 버퍼를 관리할 나만의 스레드 로컬 풀 생성
			m_pool = new (std::nothrow) LinearMemoryPool(m_buffer, threadRequirement);
		}
		else
		{
			MGF_LOG_FATAL("ThreadLocalPool: Failed to get chunk from MemoryManager!");
		}
	}

	ThreadLocalPool::TlsWrapper::~TlsWrapper()
	{
		if (m_pool)
		{
			delete m_pool;
			m_pool = nullptr;
		}
	}

	Ptr<LinearMemoryPool> ThreadLocalPool::TlsWrapper::GetPool() const noexcept
	{
		return m_pool;
	}

	Ptr<LinearMemoryPool> ThreadLocalPool::Get() noexcept
	{
		static thread_local TlsWrapper tlsWrapper;
		return tlsWrapper.GetPool();
	}
}