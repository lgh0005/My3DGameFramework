#pragma once
#include "AABB.h"

namespace MGF3D
{
	struct Ray;
	struct RaycastHit;

	class IBounding
	{
	public:
		virtual ~IBounding() = default;
		virtual bool Contains(const vec3& point) const = 0;
		virtual bool Intersects(const Ray& ray, float& outDist) const = 0;
		virtual AABB GetAABB(const mat4& transform) const = 0;

	public:
		virtual bool Raycast(const Ray& ray, RaycastHit& outHit) const = 0;
		virtual bool Raycast
		(
			const Ray& ray,
			const mat4& transform,
			const mat4& invTransform,
			RaycastHit& outHit
		) const = 0;
	};
}