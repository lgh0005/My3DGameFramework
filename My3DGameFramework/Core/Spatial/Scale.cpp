#include "CorePch.h"
#include "Scale.h"

namespace MGF3D
{
	Scale::Scale() = default;
	Scale::Scale(float uniform)
	{
		Set(uniform, uniform, uniform);
	}
	Scale::Scale(float _x, float _y, float _z)
	{
		Set(_x, _y, _z);
	}
	Scale::Scale(const vec3& v)
	{
		Set(v.x, v.y, v.z);
	}

	void Scale::Set(float _x, float _y, float _z)
	{
		x = IsValid(_x);
		y = IsValid(_y);
		z = IsValid(_z);
	}

	Scale::operator vec3() const
	{
		return vec3(x, y, z);
	}

	vec3 Scale::AsVec3() const
	{
		return vec3(x, y, z);
	}

	bool Scale::IsValid(float val)
	{
		if (Math::HasValue(val)) return val;
		return CommonUtils::Select(val >= 0.0f, Epsilon, -Epsilon);
	}
}