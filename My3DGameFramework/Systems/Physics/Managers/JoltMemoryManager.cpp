#include "PhysicsPch.h"
#include "JoltMemoryManager.h"
#include "Memory/JoltAllocator.h"
#include "Memory/JoltMemoryPool.h"

namespace MGF3D
{
	JoltMemoryManager::JoltMemoryManager() = default;
	JoltMemoryManager::~JoltMemoryManager() = default;

	bool JoltMemoryManager::Init(usize inTempCapacity)
	{
		// 1. 이중 초기화 방지
		if (m_tempPool != nullptr)
		{
			MGF_LOG_WARN("[Physics] JoltMemoryManager is already initialized.");
			return true;
		}

		// 2. 영구 할당자 후킹 (JoltAllocator)
		JPH::Allocate = JoltAllocator::Allocate;
		JPH::Free = JoltAllocator::Free;
		JPH::AlignedAllocate = JoltAllocator::AlignedAllocate;
		JPH::AlignedFree = JoltAllocator::AlignedFree;
		JPH::Reallocate = JoltAllocator::Reallocate;

		// 3. 임시 할당자 생성 (Raw Pointer 명시적 할당)
		m_tempPool = new JoltMemoryPool(inTempCapacity);

		if (!m_tempPool)
		{
			MGF_LOG_FATAL("[Physics] Failed to create JoltMemoryPool!");
			return false;
		}

		MGF_LOG_INFO("[Physics] JoltMemoryManager Initialized (TempPool: {0} MB)", inTempCapacity / (1024 * 1024));
		return true;
	}

	void JoltMemoryManager::Shutdown()
	{
		// 1. 임시 풀 명시적 파괴
		if (m_tempPool)
		{
			delete m_tempPool;
			m_tempPool = nullptr;
		}

		// 2. 후킹 해제
		JPH::Allocate = nullptr;
		JPH::Free = nullptr;
		JPH::AlignedAllocate = nullptr;
		JPH::AlignedFree = nullptr;

		MGF_LOG_INFO("[Physics] JoltMemoryManager Shutdown.");
	}

	Ptr<JPH::TempAllocator> JoltMemoryManager::GetTempAllocator() const
	{
		return m_tempPool;
	}
}