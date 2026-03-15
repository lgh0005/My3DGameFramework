#include "CorePch.h"
#include "AABB.h"

namespace MGF3D
{
	AABB::AABB() = default;
	AABB::AABB(const vec3& _min, const vec3& _max) : min(_min), max(_max) { }

	vec3 AABB::Center() const
	{
		return (max + min) * 0.5f;
	}

	vec3 AABB::Extents() const
	{
		return (max - min) * 0.5f;
	}

	bool AABB::IsValid() const
	{
		return min.x <= max.x && min.y <= max.y && min.z <= max.z;
	}

	void AABB::Encapsulate(const vec3& point)
	{
		min = Math::Min(min, point);
		max = Math::Max(max, point);
	}

	void AABB::Encapsulate(const AABB& other)
	{
		min = Math::Min(min, other.min);
		max = Math::Max(max, other.max);
	}
}