#pragma once
#include <thread>
#include <mutex>
#include <atomic>

namespace MGF3D
{
	/*=============//
	//    Atomic   //
	//=============*/
	template<typename T> using Atomic = std::atomic<T>;

	/*===========================//
	//    Thread & Sync Aliases  //
	//===========================*/
	using Thread = std::thread;
	using Mutex = std::mutex;
	using LockScope = std::lock_guard<std::mutex>;
}