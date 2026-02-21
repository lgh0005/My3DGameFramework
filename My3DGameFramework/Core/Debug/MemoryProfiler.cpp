#include "CorePch.h"
#include "MemoryProfiler.h"
#include "Debug/Logger.h"
#include "Managers/MemoryManager.h"
#include "Memory/SlabMemoryPool.h"
#include "Memory/LinearMemoryPool.h"

namespace MGF3D
{
	MemorySnapshot MemoryProfiler::m_snapshot = {};

    void MemoryProfiler::_Internal_Capture() noexcept
    {
        // 스냅샷 초기화 및 데이터 수집 시작
        m_snapshot = MemorySnapshot{};

        GetSlabMemoryUsageData();
        GetLinearMemoryUsageData();
    }

    void MemoryProfiler::GetSlabMemoryUsageData()
    {
        auto& mm = MemoryManager::Instance();
        usize currentBucketSize = 16;

        for (int32 i = 0; i < SlabBucketCount; ++i)
        {
            auto pool = mm.GetSlabMemoryPool(currentBucketSize);
            if (pool)
            {
                auto& bucket = m_snapshot.slabBuckets[i];
                bucket.bucketSize = currentBucketSize;
                bucket.usedMemory = pool->GetUsedMemory();
                bucket.totalMemory = pool->GetTotalMemory();

                if (bucket.totalMemory > 0)
                {
                    bucket.usagePercentage =
                        (static_cast<float>(bucket.usedMemory) / bucket.totalMemory) * 100.0f;
                }

                m_snapshot.totalSlabUsed += bucket.usedMemory;
                m_snapshot.totalSlabCapacity += bucket.totalMemory;
            }
            currentBucketSize <<= 1;
        }
    }

    void MemoryProfiler::GetLinearMemoryUsageData()
    {
        auto& mm = MemoryManager::Instance();
        auto linear = mm.GetLinearMemoryPool();

        if (linear)
        {
            m_snapshot.linearUsed = linear->GetUsedMemory();
            m_snapshot.linearCapacity = linear->GetTotalMemory();
        }
    }

    void MemoryProfiler::_Internal_LogStats() noexcept
    {
        MGF_LOG_WARN("============================================================");
        MGF_LOG_WARN(" [Memory Profiler Report]");
        MGF_LOG_WARN("============================================================");

        LogSlabMemoryPoolState();
        MGF_LOG_WARN("------------------------------------------------------------");
        LogLinearMemoryPoolState();

        MGF_LOG_WARN("------------------------------------------------------------");
        MGF_LOG_WARN(" >> [GRAND TOTAL] Slab: {:>8} bytes | Linear: {:>8} bytes",
            m_snapshot.totalSlabUsed, m_snapshot.linearUsed);
        MGF_LOG_WARN("============================================================");
    }

    void MemoryProfiler::LogSlabMemoryPoolState()
    {
        MGF_LOG_WARN(" <Slab Memory Pools>");
        for (int32 i = 0; i < SlabBucketCount; ++i)
        {
            const auto& bucket = m_snapshot.slabBuckets[i];
            if (bucket.totalMemory > 0)
            {
                MGF_LOG_WARN(" - Bucket [{:>4} bytes] : {:>8} / {:>8} bytes ({:>5.2f}%)",
                    bucket.bucketSize, bucket.usedMemory, bucket.totalMemory, bucket.usagePercentage);
            }
        }
        
        float totalSlabUsageRatio = 0.0f;
        if (m_snapshot.totalSlabCapacity > 0)
        {
            totalSlabUsageRatio = (static_cast<float>(m_snapshot.totalSlabUsed) / m_snapshot.totalSlabCapacity) * 100.0f;
        }

        MGF_LOG_WARN(" * Total Slab Used : {} / {} bytes ({:>5.2f}%)",
            m_snapshot.totalSlabUsed, m_snapshot.totalSlabCapacity, totalSlabUsageRatio);
    }

    void MemoryProfiler::LogLinearMemoryPoolState()
    {
        MGF_LOG_WARN(" <Linear Memory Pool (Frame)>");

        float usageRatio = 0.0f;
        if (m_snapshot.linearCapacity > 0)
        {
            usageRatio = (static_cast<float>(m_snapshot.linearUsed) / m_snapshot.linearCapacity) * 100.0f;
        }

        MGF_LOG_WARN(" - Used : {} / {} bytes ({:.2f}%)",
            m_snapshot.linearUsed, m_snapshot.linearCapacity, usageRatio);
    }

    const MemorySnapshot& MemoryProfiler::_Internal_GetSnapshot() noexcept
    {
        return m_snapshot;
    }
}