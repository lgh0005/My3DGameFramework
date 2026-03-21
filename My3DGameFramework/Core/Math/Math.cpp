#include "CorePch.h"
#include "Math.h"

namespace MGF3D
{
	/*=================//
	//   Simple Math   //
	//=================*/
	float Math::Sqrt(float v) noexcept
	{
		return glm::sqrt(v);
	}

	/*==========================//
	//   Safe math operations   //
	//==========================*/
	bool Math::HasLength(const vec3& v)
	{
		return glm::length2(v) > Epsilon2;
	}

	/*==========================//
	//   Vector Operations      //
	//==========================*/
	vec3 Math::Normalize(const vec3& v)
	{
		float len2 = glm::length2(v);
		if (len2 < Epsilon2) return vec3(0.0f);
		return v * glm::inversesqrt(len2);
	}

	quat Math::Normalize(const quat& q)
	{
		return glm::normalize(q);
	}

	/*==========================//
	//   Matrix Operations      //
	//==========================*/
	mat4 Math::Inverse(const mat4& m) noexcept
	{
		return glm::inverse(m);
	}

	mat3 Math::Inverse(const mat3& m) noexcept
	{
		return glm::inverse(m);
	}

	mat4 Math::Transpose(const mat4& m) noexcept
	{
		return glm::transpose(m);
	}

	mat3 Math::Transpose(const mat3& m) noexcept
	{
		return glm::transpose(m);
	}

	vec4 Math::GetRow(const mat4& m, int index) noexcept
	{
		return vec4(m[0][index], m[1][index], m[2][index], m[3][index]);
	}

	vec4 Math::GetColumn(const mat4& m, int index) noexcept
	{
		return m[index];
	}

	float Math::Determinant(const mat2& m) noexcept
	{
		return glm::determinant(m);
	}

	float Math::Determinant(const mat3& m) noexcept
	{
		return glm::determinant(m);
	}

	float Math::Determinant(const mat4& m) noexcept
	{
		return glm::determinant(m);
	}

	/*==========================//
	//   Vector Operations      //
	//==========================*/
	float Math::Dot(const vec3& a, const vec3& b) noexcept
	{
		return glm::dot(a, b);
	}

	vec3 Math::Cross(const vec3& a, const vec3& b) noexcept
	{
		return glm::cross(a, b);
	}

	float Math::Length(const vec3& v) noexcept
	{
		return glm::length(v);
	}

	float Math::LengthSquared(const vec3& v) noexcept
	{
		return glm::length2(v);
	}

	float Math::Distance(const vec3& a, const vec3& b) noexcept
	{
		return glm::distance(a, b);
	}

	float Math::DistanceSquared(const vec3& a, const vec3& b) noexcept
	{
		return glm::distance2(a, b);
	}

	/*===============//
	//   rotations   //
	//===============*/
	quat Math::ToQuat(const vec3& eulerDegrees) 
	{
		return quat(ToRadians(eulerDegrees));
	}

	vec3 Math::ToEuler(const quat& q) 
	{
		return ToDegrees(glm::eulerAngles(q));
	}

	/*====================//
	//   interpolations   //
	//====================*/
	quat Math::Slerp(const quat& a, const quat& b, float t) 
	{
		return glm::slerp(a, b, t);
	}
}