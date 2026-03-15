#include "CorePch.h"
#include "Ray.h"

namespace MGF3D
{
	Ray::Ray() = default;
	Ray::Ray(const vec3& _origin, const vec3& _direction)
		: origin(_origin)
	{
		SetDirection(_direction);
	}

	void Ray::SetDirection(const vec3& dir)
	{
		direction = CommonUtils::Select
		(
			Math::HasLength(dir),
			Math::Normalize(dir),
			vec3(0.0f, 0.0f, -1.0f)
		);
	}

	vec3 Ray::GetPoint(float t) const
	{
		return origin + (direction * t);
	}
}


