#include "CorePch.h"
#include "Position.h"

namespace MGF3D
{
	Position::Position() = default;
	Position::Position(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }
	Position::Position(const vec3& v) : x(v.x), y(v.y), z(v.z) { }

	void Position::operator+=(const vec3& delta)
	{
		x += delta.x; 
		y += delta.y; 
		z += delta.z;
	}

	void Position::operator-=(const vec3& delta)
	{
		x -= delta.x; 
		y -= delta.y; 
		z -= delta.z;
	}

	Position::operator vec3() const
	{
		return vec3(x, y, z);
	}
}