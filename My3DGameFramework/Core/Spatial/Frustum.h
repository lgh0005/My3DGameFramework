#pragma once
#include "CullingPlane.h"

namespace MGF3D
{
	struct AABB;

	class Frustum
	{
	public:
		enum Side { Left = 0, Right, Bottom, Top, Near, Far, MAX };
		Frustum();
		~Frustum();

		void Update(const glm::mat4& vp);
		bool IsIn(const AABB& bounds) const;
		void SetMargin(float margin) { m_margin = margin; }

	private:
		Array<CullingPlane, 6> m_planes;
		float m_margin{ 2.0f };
	};
}