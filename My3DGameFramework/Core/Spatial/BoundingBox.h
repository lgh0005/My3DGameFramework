#pragma once
#include "IBounding.h"

namespace MGF3D
{
	class BoundingBox : public IBounding
	{
	public:
		BoundingBox();
		BoundingBox(const vec3& min, const vec3& max);
		virtual ~BoundingBox();

	public:
		static BoundingBox Empty();
		static BoundingBox FromCenterExtents(const vec3& center, const vec3& extents);

		bool Contains(const vec3& point) const override;
		bool Intersects(const Ray& ray, float& outDist) const override;
		AABB GetAABB(const mat4& transform) const override;

	public:
		bool Raycast(const Ray& ray, RaycastHit& outHit) const override;
		bool Raycast
		(
			const Ray& ray, 
			const mat4& transform, 
			const mat4& invTransform, 
			RaycastHit& outHit
		) const override;

	public:
		BoundingBox Transformed(const mat4& mat) const;
		BoundingBox Combined(const BoundingBox& other) const;

	public:
		vec3 GetCenter()  const { return m_data.Center(); }
		vec3 GetExtents() const { return m_data.Extents(); }

	private:
		AABB m_data;
	};
}