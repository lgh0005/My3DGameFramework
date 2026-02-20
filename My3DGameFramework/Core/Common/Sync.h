#pragma once
#include <mutex>
#include <thread>
#include <condition_variable>

namespace MGF3D
{
	/*===========================//
	//   Thread & Sync Aliases   //
	//===========================*/
	using Mutex = std::mutex;

	using Lock = std::lock_guard<Mutex>;
	using UniqueLock = std::unique_lock<Mutex>;

	using ConditionVariable = std::condition_variable;
	using Thread = std::thread;
}