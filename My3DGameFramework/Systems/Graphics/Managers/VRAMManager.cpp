#include "GraphicsPch.h"
#include "VRAMManager.h"
#include "VRAM/VRAMPool.h"

namespace MGF3D
{
	VRAMManager::VRAMManager() = default;
	VRAMManager::~VRAMManager() = default;

	bool VRAMManager::Init(uint64 staticPoolSize, uint64 dynamicPoolSize)
	{
        // TODO : 이건 모듈 실행 초기에 어딘가에서 받아와서 전역적으로 사용할 수 있도록
        // 보장을 해줘야 함.
        //GLint minUboAlignment = 0;
        //glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &minUboAlignment);

		m_staticVRAMPool = MakeUnique<VRAMPool>(staticPoolSize);
		m_dynamicVRAMPool = MakeUnique<VRAMPool>(dynamicPoolSize);
		return (m_staticVRAMPool && m_dynamicVRAMPool);
	}

	void VRAMManager::Shutdown()
	{
		m_staticVRAMPool.Reset();
		m_dynamicVRAMPool.Reset();
		MGF_LOG_INFO("VRAMManager Shutdown complete.");
	}

	VRAMAllocation VRAMManager::Allocate
	(
		VRAMAllocation::PoolType type, 
		uint64 size, 
		uint32 alignment
	)
	{
        switch (type)
        {
            case VRAMAllocation::PoolType::Static:
                return AllocateFromPool(m_staticVRAMPool.Get(), type, size, alignment);

            case VRAMAllocation::PoolType::Dynamic:
                return AllocateFromPool(m_dynamicVRAMPool.Get(), type, size, alignment);

            default:
                MGF_LOG_ERROR("VRAMManager Allocation failed. Unknown PoolType.");
                return VRAMAllocation();
        }
	}

    void VRAMManager::Deallocate(VRAMAllocation& alloc)
    {
        if (!alloc.IsValid()) return;

        switch (alloc.GetPoolType())
        {
            case VRAMAllocation::PoolType::Static:
                DeallocateFromPool(m_staticVRAMPool.Get(), alloc.GetOffset());
                break;

            case VRAMAllocation::PoolType::Dynamic:
                DeallocateFromPool(m_dynamicVRAMPool.Get(), alloc.GetOffset());
                break;

            default:
                break;
        }

        alloc.Invalidate();
    }

	void VRAMManager::UploadData(const VRAMAllocation& alloc, const void* data)
	{
        if (!alloc.IsValid() || data == nullptr) return;

        switch (alloc.GetPoolType())
        {
            case VRAMAllocation::PoolType::Static:
                UploadToPool(m_staticVRAMPool.Get(), alloc.GetOffset(), data, alloc.GetSize());
                break;

            case VRAMAllocation::PoolType::Dynamic:
                UploadToPool(m_dynamicVRAMPool.Get(), alloc.GetOffset(), data, alloc.GetSize());
                break;

            default:
                break;
        }
	}

    /*=================================================//
    //   Internal VRAM Allocate & Deallocate methods   //
    //=================================================*/
    VRAMAllocation VRAMManager::AllocateFromPool
    (
        Ptr<VRAMPool> pool, 
        VRAMAllocation::PoolType type, 
        uint64 size, 
        uint32 alignment
    )
    {
        MGF_ASSERT(pool != nullptr, "VRAMPool is not initialized!");

        uint64 offset = pool->Allocate(size, alignment);
        if (offset == static_cast<uint64>(-1))
        {
            MGF_LOG_ERROR("[VRAM] Allocation failed. Size: {0}", size);
            return VRAMAllocation();
        }

        return VRAMAllocation(offset, size, pool->GetBufferHandle(), type);
    }

    void VRAMManager::DeallocateFromPool(Ptr<VRAMPool> pool, uint64 offset)
    {
        MGF_ASSERT(pool != nullptr, "VRAMPool is not initialized!");
        pool->Deallocate(offset);
    }

    void VRAMManager::UploadToPool(Ptr<VRAMPool> pool, uint64 offset, const void* data, uint64 size)
    {
        MGF_ASSERT(pool != nullptr, "VRAMPool is not initialized!");
        pool->UploadData(offset, data, size);
    }

    /*====================================//
    //   VRAMPool usage profile methods   //
    //====================================*/
    uint64 VRAMManager::GetStaticPoolUsed() const
    {
        return m_staticVRAMPool ? m_staticVRAMPool->GetUsedMemory() : 0;
    }

    uint64 VRAMManager::GetStaticPoolTotal() const
    {
        return m_staticVRAMPool ? m_staticVRAMPool->GetTotalMemory() : 0;
    }

    uint64 VRAMManager::GetDynamicPoolUsed() const
    {
        return m_dynamicVRAMPool ? m_dynamicVRAMPool->GetUsedMemory() : 0;
    }

    uint64 VRAMManager::GetDynamicPoolTotal() const
    {
        return m_dynamicVRAMPool ? m_dynamicVRAMPool->GetTotalMemory() : 0;
    }
}