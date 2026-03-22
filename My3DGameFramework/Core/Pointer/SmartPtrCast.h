#pragma once

namespace MGF3D
{
	/*==============================//
	//   SharedPtr casting methods  //
	//==============================*/
	template<typename T, typename U>
	SharedPtr<T> StaticPtrCast(const SharedPtr<U>& ptr);

	template<typename T, typename U>
	SharedPtr<T> ConstPtrCast(const SharedPtr<U>& ptr);

	template<typename T, typename U>
	SharedPtr<T> ReinterpretPtrCast(const SharedPtr<U>& ptr);

	/*==============================//
	//   UniquePtr casting methods  //
	//==============================*/
	template<typename T, typename U>
	UniquePtr<T> StaticPtrCast(UniquePtr<U>&& ptr);

	template<typename T, typename U>
	UniquePtr<T> ReinterpretPtrCast(UniquePtr<U>&& ptr);
}

#include "Pointer/SmartPtrCast.inl"