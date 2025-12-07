#pragma once
#include "Core/RenderPass.h"
#include "Graphics/Frustum.h"

// TODO : '이득우 게임 수학'에서 얻은 통찰인데
// 컬링을 할 때 물체의 컬링 범위(여기서는 RenderBound)를 그냥 좀 더 키우면 됨
// 절두체에서 뭔가를 하려고 하면 조금은 해결은 될지라도 그래도 크게 해결되는 것
// 같지는 않으니
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