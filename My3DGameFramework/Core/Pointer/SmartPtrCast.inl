#include "SmartPtrCast.h"
#pragma once

namespace MGF3D
{
	/*==============================//
	//   SharedPtr casting methods  //
	//==============================*/
	template<typename T, typename U>
	inline SharedPtr<T> StaticPtrCast(const SharedPtr<U>& ptr)
	{
		return SharedPtr<T>();
	}

	template<typename T, typename U>
	inline SharedPtr<T> ConstPtrCast(const SharedPtr<U>& ptr)
	{
		return SharedPtr<T>();
	}

	template<typename T, typename U>
	inline SharedPtr<T> ReinterpretPtrCast(const SharedPtr<U>& ptr)
	{
		return SharedPtr<T>();
	}

	/*==============================//
	//   UniquePtr casting methods  //
	//==============================*/
	template<typename T, typename U>
	inline UniquePtr<T> StaticPtrCast(UniquePtr<U>&& ptr)
	{
		return UniquePtr<T>();
	}

	template<typename T, typename U>
	inline UniquePtr<T> ReinterpretPtrCast(UniquePtr<U>&& ptr)
	{
		return UniquePtr<T>();
	}
}