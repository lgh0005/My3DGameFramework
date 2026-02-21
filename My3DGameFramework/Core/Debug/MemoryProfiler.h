#pragma once

namespace MGF3D
{
	struct SlabStats
	{
		usize bucketSize{ 0 };
		usize usedMemory{ 0 };
		usize totalMemory{ 0 };
		float usagePercentage{ 0.0f };
	};

	struct MemorySnapshot
	{
		// Slab 영역 통계 (버킷 인덱스별)
		SlabStats slabBuckets[MGF3D::SlabBucketCount];

		// 전체 요약
		usize totalSlabUsed{ 0 };
		usize totalSlabCapacity{ 0 };

		// Linear 영역 통계
		usize linearUsed{ 0 };
		usize linearCapacity{ 0 };
	};

	class MemoryProfiler
	{
		DECLARE_UTILITIES(MemoryProfiler)

	public:
		static void _Internal_Capture() noexcept;
		static void _Internal_LogStats() noexcept;
		static const MemorySnapshot& _Internal_GetSnapshot() noexcept;

	private:
		static void GetSlabMemoryUsageData();
		static void GetLinearMemoryUsageData();
		static void LogSlabMemoryPoolState();
		static void LogLinearMemoryPoolState();

#ifdef _DEBUG
	private:
		static MemorySnapshot m_snapshot;
#endif
	};
}

/*=============================//
//    Memory Profiler Macros   //
//=============================*/
#ifdef _DEBUG
#define MGF_MEMORY_PROFILE_CAPTURE()    MGF3D::MemoryProfiler::_Internal_Capture()
#define MGF_MEMORY_LOG_STATS()          MGF3D::MemoryProfiler::_Internal_LogStats()
#define MGF_MEMORY_GET_SNAPSHOT()       MGF3D::MemoryProfiler::_Internal_GetSnapshot()
#else
#define MGF_MEMORY_PROFILE_CAPTURE()    ((void)0)
#define MGF_MEMORY_LOG_STATS()          ((void)0)
#define MGF_MEMORY_GET_SNAPSHOT()       MGF3D::MemorySnapshot{} 
#endif