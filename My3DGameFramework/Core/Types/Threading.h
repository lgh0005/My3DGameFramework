#pragma once
#include <thread>
#include <mutex>

namespace MGF3D
{
	/*===========================//
	//    Thread & Sync Aliases  //
	//===========================*/
	using Thread = std::thread;
	using Mutex = std::mutex;
	using LockScope = std::lock_guard<std::mutex>;
}