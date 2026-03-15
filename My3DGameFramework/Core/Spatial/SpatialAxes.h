#pragma once

namespace MGF3D
{
	class SpatialAxes
	{
	public:
		SpatialAxes();

		void Update(const quat& rotation);

		const vec3& Forward()	const;
		const vec3& Up()		const;
		const vec3& Right()		const;

	private:
		vec3 m_forward	{ 0.0f, 0.0f, -1.0f };
		vec3 m_up		{ 0.0f, 1.0f, 0.0f };
		vec3 m_right	{ 1.0f, 0.0f, 0.0f };
	};
}