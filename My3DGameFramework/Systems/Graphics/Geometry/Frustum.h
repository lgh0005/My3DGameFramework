#pragma once
#include "Geometry/RenderBounds.h"
#include "Geometry/CullingPlane.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Frustum)
	class Frustum
	{
		enum EPlaneSide { Left = 0, Right, Bottom, Top, Near, Far };

	public:
		~Frustum();
		Frustum();

		void Update(const mat4& vp);
		void SetMargin(float margin) { m_margin = margin; }
		bool CheckBounds(const RenderBounds& bounds) const;

	private:
		void Init();

		CullingPlane m_planes[6];
		float m_margin{ 2.0f };
	};
}
