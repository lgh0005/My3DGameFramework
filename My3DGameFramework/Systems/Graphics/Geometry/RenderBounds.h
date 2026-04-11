#pragma once

namespace MGF3D
{
	class RenderBounds
	{
	public:
		RenderBounds();
		RenderBounds(const vec3& center, const vec3& extents);
		template<typename T> RenderBounds(const Vector<T>& vertices);
		~RenderBounds();

	public:
		RenderBounds Transform(const mat4& mat) const;
		RenderBounds Union(const RenderBounds& other) const;
		void SetFromMinMax(const vec3& min, const vec3& max);
		template <typename T> void SetFromVertices(const Vector<T>& vertices);

	public:
		const vec3& GetCenter() const { return m_center; }
		const vec3& GetExtents() const { return m_extents; }
		vec3 GetMin() const { return m_center - m_extents; }
		vec3 GetMax() const { return m_center + m_extents; }

	private:
		vec3 m_center{ 0.0f };
		vec3 m_extents{ 0.0f };
	};
}

#include "Geometry/RenderBounds.inl"