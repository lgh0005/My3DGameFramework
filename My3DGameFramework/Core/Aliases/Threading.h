#pragma once
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace MGF3D
{
	// TODO : 이후에는 진지하게 class로 래핑을 해둬야함.

	/*===========================//
	//   Thread & Sync Aliases   //
	//===========================*/
	template<typename T>
	using Atomic = std::atomic<T>;

	using Mutex = std::mutex;
	using Lock = std::lock_guard<Mutex>;
	using UniqueLock = std::unique_lock<Mutex>;
	using ConditionVariable = std::condition_variable;
	using Thread = std::thread;
}

#define MGF_LOCK_SCOPE(mtx) Lock lock##__LINE__(mtx)