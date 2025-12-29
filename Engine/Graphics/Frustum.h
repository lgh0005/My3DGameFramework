#pragma once
#include "Graphics/Geometry.h"

CLASS_PTR(Frustum)
class Frustum
{
	enum Side { Left = 0, Right, Bottom, Top, Near, Far };

public:
	~Frustum();
	static FrustumUPtr Create();
	void Update(const glm::mat4& vp);
	void SetMargin(float margin) { m_margin = margin; }
	bool CheckBounds(const RenderBounds& bounds) const;

private:
	Frustum();
	void Init();

	CullingPlane m_planes[6];
	float m_margin	{ 2.0f };
};