#include "CorePch.h"
#include "Math.h"

namespace MGF3D
{
	/*==========================//
	//   Safe math operations   //
	//==========================*/
	bool Math::HasLength(const vec3& v)
	{
		return glm::length2(v) > Epsilon2;
	}

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