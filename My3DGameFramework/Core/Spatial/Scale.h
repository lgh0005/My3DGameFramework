#pragma once

namespace MGF3D
{
	struct Scale
	{
		float x{ 1.0f };
		float y{ 1.0f };
		float z{ 1.0f };

		Scale();
		Scale(float uniform);
		Scale(float _x, float _y, float _z);
		Scale(const vec3& v);

		void Set(float _x, float _y, float _z);
		void Set(float uniform) { Set(uniform, uniform, uniform); }

		operator vec3() const;

		vec3 AsVec3() const;

	private:
		bool IsValid(float val);
	};
}