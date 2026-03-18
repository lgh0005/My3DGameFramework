#pragma once

namespace MGF3D
{
    struct VRAMPoolStats
    {
        uint64 usedMemory{ 0 };
        uint64 totalMemory{ 0 };
        float  usagePercentage{ 0.0f };
    };

    struct VRAMSnapshot
    {
        VRAMPoolStats staticPool;
        VRAMPoolStats dynamicPool;

        uint64 totalUsed{ 0 };
        uint64 totalCapacity{ 0 };
    };

    class VRAMProfiler
    {
        MGF_DECLARE_UTILITIES(VRAMProfiler)

    public:
        static void _Internal_Capture() noexcept;
        static void _Internal_LogStats() noexcept;
        static const VRAMSnapshot& _Internal_GetSnapshot() noexcept;

    private:
        static void GetPoolUsageData();
        static void LogPoolState();

#ifdef _DEBUG
    private:
        static VRAMSnapshot m_snapshot;
#endif
    };
}

/*=============================//
//   VRAM Profiler Macros      //
//=============================*/
#ifdef _DEBUG
#define MGF_VRAM_PROFILE_CAPTURE()    MGF3D::VRAMProfiler::_Internal_Capture()
#define MGF_VRAM_LOG_STATS()          MGF3D::VRAMProfiler::_Internal_LogStats()
#define MGF_VRAM_GET_SNAPSHOT()       MGF3D::VRAMProfiler::_Internal_GetSnapshot()
#else
#define MGF_VRAM_PROFILE_CAPTURE()    ((void)0)
#define MGF_VRAM_LOG_STATS()          ((void)0)
#define MGF_VRAM_GET_SNAPSHOT()       MGF3D::VRAMSnapshot{} 
#endif