#pragma once
#include "IBounding.h"

namespace MGF3D
{
	class BoundingSphere : public IBounding
	{
	public:
		BoundingSphere();
		BoundingSphere(const vec3& center, float radius);
		virtual ~BoundingSphere();

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
		BoundingSphere Transformed(const mat4& mat) const;

	public:
		vec3 GetCenter()  const { return center; }
		float GetRadius() const { return radius; }

	private:
		vec3  center{ 0.0f };
		float radius{ 0.0f };
	};
}