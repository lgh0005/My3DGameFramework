#pragma once

namespace MGF3D
{
	/*=================//
	//   Simple Math   //
	//=================*/
	template<typename T>
	inline constexpr T Math::Min(const T& a, const T& b) noexcept
	{
		MGF_STATIC_ASSERT(std::is_arithmetic_v<T>, "T must be an arithmetic type.");
		return glm::min<T>(a, b);
	}

	template<typename T>
	inline constexpr T Math::Max(const T& a, const T& b) noexcept
	{
		MGF_STATIC_ASSERT(std::is_arithmetic_v<T>, "T must be an arithmetic type.");
		return glm::max<T>(a, b);
	}

	template<>
	inline constexpr vec2 Math::Min<vec2>(const vec2& a, const vec2& b) noexcept
	{
		return vec2(Min<float>(a.x, b.x), Min<float>(a.y, b.y));
	}
	template<>
	inline constexpr vec2 Math::Max<vec2>(const vec2& a, const vec2& b) noexcept
	{
		return vec2(Max<float>(a.x, b.x), Max<float>(a.y, b.y));
	}

	template<>
	inline constexpr vec3 Math::Min<vec3>(const vec3& a, const vec3& b) noexcept
	{
		return vec3(Min<float>(a.x, b.x), Min<float>(a.y, b.y), Min<float>(a.z, b.z));
	}
	template<>
	inline constexpr vec3 Math::Max<vec3>(const vec3& a, const vec3& b) noexcept
	{
		return vec3(Max<float>(a.x, b.x), Max<float>(a.y, b.y), Max<float>(a.z, b.z));
	}

	template<>
	inline constexpr vec4 Math::Min<vec4>(const vec4& a, const vec4& b) noexcept
	{
		return vec4(Min<float>(a.x, b.x), Min<float>(a.y, b.y), Min<float>(a.z, b.z), Min<float>(a.w, b.w));
	}
	template<>
	inline constexpr vec4 Math::Max<vec4>(const vec4& a, const vec4& b) noexcept
	{
		return vec4(Max<float>(a.x, b.x), Max<float>(a.y, b.y), Max<float>(a.z, b.z), Max<float>(a.w, b.w));
	}

	template<typename T>
	inline constexpr T Math::Clamp(const T& v, const T& lo, const T& hi) noexcept
	{
		return (v < lo) ? lo : (hi < v) ? hi : v;
	}

	template<typename T>
	inline constexpr T Math::Saturate(const T& v) noexcept
	{
		return Clamp(v, static_cast<T>(0), static_cast<T>(1));
	}

	template<typename T>
	inline constexpr T Math::Abs(const T& v) noexcept
	{
		return glm::abs(v);
	}

	template<typename T>
	inline constexpr T Math::Square(const T& v) noexcept
	{
		return v * v;
	}

	/*==========================//
	//   Safe math operations   //
	//==========================*/
	inline bool Math::IsNearlyZero(float v) noexcept
	{
		return Abs(v) < EPSILON;
	}

	inline bool Math::HasValue(float v) noexcept
	{
		return Abs(v) >= EPSILON;
	}

	/*========================//
	//   rad or degree angle  //
	//========================*/
	inline float Math::ToRadians(float degrees) noexcept
	{
		return degrees * DEG_TO_RAD;
	}

	inline float Math::ToDegrees(float radians) noexcept
	{
		return radians * DEG_TO_RAD;
	}

	inline vec3 Math::ToRadians(const vec3& degrees) noexcept
	{
		return degrees * DEG_TO_RAD;
	}

	inline vec3 Math::ToDegrees(const vec3& radians) noexcept
	{
		return radians * DEG_TO_RAD;
	}

	/*====================//
	//   interpolations   //
	//====================*/
	inline constexpr float Math::Lerp(float a, float b, float f) noexcept
	{
		return a + f * (b - a);
	}
}