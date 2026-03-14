#pragma once

namespace MGF3D
{
	struct Position
	{
		float x	{ 0.0f };
		float y	{ 0.0f };
		float z	{ 0.0f };

		Position();
		Position(float _x, float _y, float _z);
		Position(const vec3& v);

		void operator+=(const vec3& delta);
		void operator-=(const vec3& delta);
		
		operator vec3() const;
	};
}