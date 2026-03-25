#include "GraphicsPch.h"
#include "VRAMProfiler.h"
#include "Debug/Logger.h"
#include "Managers/VRAMManager.h"

namespace MGF3D
{
	VRAMSnapshot VRAMProfiler::m_snapshot = {};

    void VRAMProfiler::_Internal_Capture() noexcept
    {
        m_snapshot = VRAMSnapshot{};
        GetPoolUsageData();
    }

    void VRAMProfiler::GetPoolUsageData()
    {
        // Static Pool 데이터 수집
        m_snapshot.staticPool.usedMemory = MGF_VRAM.GetStaticPoolUsed();
        m_snapshot.staticPool.totalMemory = MGF_VRAM.GetStaticPoolTotal();
        if (m_snapshot.staticPool.totalMemory > 0)
        {
            m_snapshot.staticPool.usagePercentage =
                (static_cast<float>(m_snapshot.staticPool.usedMemory) / m_snapshot.staticPool.totalMemory) * 100.0f;
        }

        // Dynamic Pool 데이터 수집
        m_snapshot.dynamicPool.usedMemory = MGF_VRAM.GetDynamicPoolUsed();
        m_snapshot.dynamicPool.totalMemory = MGF_VRAM.GetDynamicPoolTotal();
        if (m_snapshot.dynamicPool.totalMemory > 0)
        {
            m_snapshot.dynamicPool.usagePercentage =
                (static_cast<float>(m_snapshot.dynamicPool.usedMemory) / m_snapshot.dynamicPool.totalMemory) * 100.0f;
        }

        // 전체 요약
        m_snapshot.totalUsed = m_snapshot.staticPool.usedMemory + m_snapshot.dynamicPool.usedMemory;
        m_snapshot.totalCapacity = m_snapshot.staticPool.totalMemory + m_snapshot.dynamicPool.totalMemory;
    }

    void VRAMProfiler::_Internal_LogStats() noexcept
    {
        MGF_LOG_WARN("============================================================");
        MGF_LOG_WARN(" [VRAM Profiler Report]");
        MGF_LOG_WARN("============================================================");

        LogPoolState();

        MGF_LOG_WARN("------------------------------------------------------------");
        MGF_LOG_WARN(" >> [GRAND TOTAL] VRAM Used: {:>8} / {:>8} bytes ({:>5.2f}%)",
            m_snapshot.totalUsed, m_snapshot.totalCapacity,
            m_snapshot.totalCapacity > 0 ? (static_cast<float>(m_snapshot.totalUsed) / m_snapshot.totalCapacity) * 100.0f : 0.0f);
        MGF_LOG_WARN("============================================================");
    }

    void VRAMProfiler::LogPoolState()
    {
        MGF_LOG_WARN(" <VRAM Pools>");

        MGF_LOG_WARN(" - [Static]  : {:>8} / {:>8} bytes ({:>5.2f}%)",
            m_snapshot.staticPool.usedMemory, m_snapshot.staticPool.totalMemory, m_snapshot.staticPool.usagePercentage);

        MGF_LOG_WARN(" - [Dynamic] : {:>8} / {:>8} bytes ({:>5.2f}%)",
            m_snapshot.dynamicPool.usedMemory, m_snapshot.dynamicPool.totalMemory, m_snapshot.dynamicPool.usagePercentage);
    }

    const VRAMSnapshot& VRAMProfiler::_Internal_GetSnapshot() noexcept
    {
        return m_snapshot;
    }
}