#pragma once

namespace MGF3D
{
	class Math
	{
		MGF_DECLARE_UTILITIES(Math)

	/*=================//
	//   Simple Math   //
	//=================*/
	public:
		template<typename T> static constexpr T Min(const T& a, const T& b) noexcept;
		template<typename T> static constexpr T Max(const T& a, const T& b) noexcept;
		template<typename T> static constexpr T Abs(const T& v) noexcept;

	/*==========================//
	//   Safe math operations   //
	//==========================*/
	public:
		static bool HasLength(const vec3& v);
		static bool IsNearlyZero(float v) noexcept;
		static bool HasValue(float v) noexcept;

		static vec3 Normalize(const vec3& v);
		static quat Normalize(const quat& q);

	/*========================//
	//   rad or degree angle  //
	//========================*/
	public:
		static constexpr float ToRadians(float degrees) noexcept;
		static constexpr float ToDegrees(float radians) noexcept;
		static vec3 ToRadians(const vec3& degrees) noexcept;
		static vec3 ToDegrees(const vec3& radians) noexcept;

	/*===============//
	//   rotations   //
	//===============*/
	public:
		static quat ToQuat(const vec3& eulerDegrees) ;
		static vec3 ToEuler(const quat& q);

	/*====================//
	//   interpolations   //
	//====================*/
	public:
		static constexpr float Lerp(float a, float b, float f) noexcept;
		static quat Slerp(const quat& a, const quat& b, float t);
	};
}

#include "Math.inl"