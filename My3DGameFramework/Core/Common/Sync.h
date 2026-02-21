#pragma once
#include <mutex>
#include <thread>
#include <condition_variable>
#include <future>

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

	/*===========================//
	//   Async Task Aliases      //
	//===========================*/
	template<typename T>
	using Future = std::future<T>;

	template<typename T>
	using SharedFuture = std::shared_future<T>;

	template<typename T>
	using Promise = std::promise<T>;

	template<typename Signature>
	using PackagedTask = std::packaged_task<Signature>;
}