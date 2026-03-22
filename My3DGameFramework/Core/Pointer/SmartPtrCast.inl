#include "SmartPtrCast.h"
#pragma once

namespace MGF3D
{
	/*==========================================//
	//      SharedPtr Casting Utilities         //
	//==========================================*/
	template<typename T, typename U>
	inline SharedPtr<T> StaticSharedCast(const SharedPtr<U>& ptr) noexcept
	{
		return SharedPtr<T>(static_cast<T*>(ptr.Get()));
	}

	template<typename T, typename U>
	inline SharedPtr<T> ReinterpretSharedCast(const SharedPtr<U>& ptr) noexcept
	{
		return SharedPtr<T>(reinterpret_cast<T*>(ptr.Get()));
	}

	template<typename T, typename U>
	inline SharedPtr<T> ConstSharedCast(const SharedPtr<U>& ptr) noexcept
	{
		return SharedPtr<T>(const_cast<T*>(ptr.Get()));
	}

	/*===========================================//
	//      Pointer Type Casting Utilities       //
	//===========================================*/
	template<typename T>
	constexpr std::remove_reference_t<T>&& Move(T&& arg) noexcept
	{
		return static_cast<std::remove_reference_t<T>&&>(arg);
	}

	template<typename T>
	constexpr T&& Forward(std::remove_reference_t<T>& arg) noexcept
	{
		return static_cast<T&&>(arg);
	}

	template<typename T>
	constexpr T&& Forward(std::remove_reference_t<T>&& arg) noexcept
	{
		MGF_STATIC_ASSERT
		(
			!std::is_lvalue_reference_v<T>, 
			"MGF3D::Forward: Cannot forward an rvalue as an lvalue."
		);
		return static_cast<T&&>(arg);
	}
}