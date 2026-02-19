#include "CorePch.h"
#include "Math.h"

namespace MGF3D
{
	glm::vec3 Math::Normalize(const vec3& v)
	{
		float len2 = glm::length2(v);
		if (len2 < EPSILON2) return vec3(0.0f);
		return v * glm::inversesqrt(len2);
	}
}