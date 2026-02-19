#pragma once

namespace MGF3D
{
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
}