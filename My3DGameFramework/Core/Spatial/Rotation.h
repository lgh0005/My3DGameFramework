#pragma once

namespace MGF3D
{
	class Rotation
	{
		quat value{ 1.0f, 0.0f, 0.0f, 0.0f };

		Rotation();
		Rotation(const quat& q);
		Rotation(const vec3& eulerDegrees);

		void Rotate(const vec3& deltaDegrees);
		vec3 ToEuler() const;

		operator quat() const;
		void operator*=(const quat& q);
	};
}