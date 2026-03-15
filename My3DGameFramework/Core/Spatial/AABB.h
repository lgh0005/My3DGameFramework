#pragma once

namespace MGF3D
{
	struct AABB
	{
		vec3 min{ FloatMax };
		vec3 max{ FloatLowest };

		AABB();
		AABB(const vec3& _min, const vec3& _max);

		vec3 Center()  const;
		vec3 Extents() const;
		bool IsValid() const;

		void Encapsulate(const vec3& point);
		void Encapsulate(const AABB& other);
	};
}