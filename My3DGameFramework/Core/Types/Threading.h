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
	using LockScope = std::scoped_lock<Mutex>;

	/*=========================//
	//    Producer & Consumer  //
	//=========================*/
	using UniqueLock = std::unique_lock<Mutex>;
	using ConditionVariable = std::condition_variable;
}