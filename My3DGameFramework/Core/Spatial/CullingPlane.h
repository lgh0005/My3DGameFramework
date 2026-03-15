#pragma once

namespace MGF3D
{
	struct Ray;

	class CullingPlane
	{
	public:
		CullingPlane();
		CullingPlane(const vec4& equation);
		CullingPlane(const vec3& normal, float dist);
		CullingPlane(const vec3& normal, const vec3& pointOnPlane);
		~CullingPlane();

	public:
		void Normalize();
		float GetDistanceToPoint(const vec3& point) const;
		bool Intersects(const Ray& ray, float& outDist) const;
		const vec3& Normal() const { return m_normal; }
		float Distance() const { return m_distance; }

	private:
		vec3 m_normal		{ 0.0f, 1.0f, 0.0f };
		float m_distance	{ 0.0f };
	};
}