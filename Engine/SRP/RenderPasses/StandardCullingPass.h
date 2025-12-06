#pragma once
#include "Core/RenderPass.h"
#include "Graphics/Frustum.h"

CLASS_PTR(StandardCullingPass)
class StandardCullingPass : public ContextRenderPass
{
public:
	static StandardCullingPassUPtr Create();
	virtual ~StandardCullingPass();

	virtual void Render(RenderContext* context) override;

private:
	StandardCullingPass();
	bool Init();

	FrustumUPtr m_frustum;
};