#include "CorePch.h"
#include "Rotation.h"

namespace MGF3D
{
	Rotation::Rotation() = default;
	Rotation::Rotation(const quat& q) : value(Math::Normalize(q)) { }
	Rotation::Rotation(const vec3& eulerDegrees)
	{
		value = Math::ToQuat(eulerDegrees);
	}

	void Rotation::Rotate(const vec3& deltaDegrees)
	{
		quat delta = Math::ToQuat(deltaDegrees);
		value = Math::Normalize(value * delta);
	}

	vec3 Rotation::ToEuler() const
	{
		return Math::ToEuler(value);
	}

	Rotation::operator quat() const
	{
		return value;
	}

	void Rotation::operator*=(const quat& q)
	{
		value = Math::Normalize(value * q);
	}
}