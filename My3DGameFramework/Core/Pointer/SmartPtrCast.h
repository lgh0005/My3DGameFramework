#pragma once

namespace MGF3D
{
	/*==========================================//
	//      SharedPtr Casting Utilities         //
	//==========================================*/
	template <typename T, typename U>
	[[nodiscard]] inline SharedPtr<T> StaticSharedCast(const SharedPtr<U>& ptr) noexcept;

	template <typename T, typename U>
	[[nodiscard]] inline SharedPtr<T> ReinterpretSharedCast(const SharedPtr<U>& ptr) noexcept;

	template <typename T, typename U>
	[[nodiscard]] inline SharedPtr<T> ConstSharedCast(const SharedPtr<U>& ptr) noexcept;

	/*===========================================//
	//      Pointer Type Casting Utilities       //
	//===========================================*/
	template <typename T> constexpr std::remove_reference_t<T>&& Move(T&& arg) noexcept;
	template <typename T> constexpr T&& Forward(std::remove_reference_t<T>& arg) noexcept;
	template <typename T> constexpr T&& Forward(std::remove_reference_t<T>&& arg) noexcept;
}

#include "Pointer/SmartPtrCast.inl"