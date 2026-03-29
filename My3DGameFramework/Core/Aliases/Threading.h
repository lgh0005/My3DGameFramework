#pragma once
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <condition_variable>

namespace MGF3D
{
	/*===========================//
	//   Thread & Sync Aliases   //
	//===========================*/
	template<typename T>
	using Atomic = std::atomic<T>;

	using Mutex = std::mutex;
	using Lock = std::lock_guard<Mutex>;
	using UniqueLock = std::unique_lock<Mutex>;

	using SharedMutex = std::shared_mutex;
	using SharedLock = std::shared_lock<SharedMutex>;
	using SharedWriteLock = std::unique_lock<SharedMutex>;

	using ConditionVariable = std::condition_variable;
	using Thread = std::thread;

	namespace ThisThread
	{
		inline void Yield() noexcept;
		inline void SleepFor(unsigned int milliseconds) noexcept;
		inline Thread::id GetId() noexcept;
	}
}

#define MGF_LOCK_SCOPE(mtx) Lock lock##__LINE__(mtx)
#define MGF_SHARED_LOCK_SCOPE(mtx) SharedLock sharedLock##__LINE__(mtx)
#define MGF_SHARED_WRITE_LOCK_SCOPE(mtx) SharedWriteLock sharedWriteLock##__LINE__(mtx)

#include "Aliases/Threading.inl"