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
		template<typename T> static constexpr T Square(const T& v) noexcept;
		static float Sqrt(float v) noexcept;

	/*==========================//
	//   Safe math operations   //
	//==========================*/
	public:
		static bool HasLength(const vec3& v);
		static bool IsNearlyZero(float v) noexcept;
		static bool HasValue(float v) noexcept;
	
	/*==========================//
	//   Matrix Operations      //
	//==========================*/
	public:
		static mat4 Inverse(const mat4& m) noexcept;
		static mat3 Inverse(const mat3& m) noexcept;
		static mat4 Transpose(const mat4& m) noexcept;
		static mat3 Transpose(const mat3& m) noexcept;
		static vec4 GetRow(const mat4& m, int index) noexcept;
		static vec4 GetColumn(const mat4& m, int index) noexcept;

	/*==========================//
	//   Vector Operations      //
	//==========================*/
	public:
		static float Dot(const vec3& a, const vec3& b) noexcept;
		static vec3  Cross(const vec3& a, const vec3& b) noexcept;
		static float Length(const vec3& v) noexcept;
		static float LengthSquared(const vec3& v) noexcept;
		static float Distance(const vec3& a, const vec3& b) noexcept;
		static float DistanceSquared(const vec3& a, const vec3& b) noexcept;
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