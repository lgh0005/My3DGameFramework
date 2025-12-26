#pragma once
#include "Core/RenderPass.h"
#include "Graphics/Frustum.h"

CLASS_PTR(CullingPass)
class CullingPass : public ContextRenderPass
{
public:
	static CullingPassUPtr Create();
	virtual ~CullingPass();

	virtual void Render(RenderContext* context) override;

private:
	CullingPass();
	bool Init();

	FrustumUPtr m_frustum;
};