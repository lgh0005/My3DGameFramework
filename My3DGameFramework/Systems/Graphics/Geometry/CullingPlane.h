#pragma once

namespace MGF3D
{
	class CullingPlane
	{
	public:
		CullingPlane();
		CullingPlane(const vec4& eq);
		CullingPlane(const vec3& normal, float distance);
		~CullingPlane();

	public:
		void Normalize();
		float GetDistanceToPoint(const vec3& point) const;
		const vec3& GetNormal() const { return m_normal; }
		float GetDistance() const { return m_distance; }

	private:
		vec3	  m_normal;
		float     m_distance;
	};
}