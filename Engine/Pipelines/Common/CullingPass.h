#pragma once
#include "Graphics/RenderPass.h"
#include "Graphics/Frustum.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(ComponentRegistry)
CLASS_PTR(RenderContext)
#pragma endregion

CLASS_PTR(CullingPass)
class CullingPass : public ContextRenderPass
{
public:
	virtual ~CullingPass();
	static CullingPassUPtr Create();
	virtual void Render(RenderContext* context) override;

/*=====================//
//   culling methods   //
//=====================*/
private:
	void CullStaticMeshRenderers(ComponentRegistry* registry, RenderContext* context);
	void CullSkinnedMeshRenderers(ComponentRegistry* registry, RenderContext* context);
	void CullMeshOutlines(ComponentRegistry* registry, RenderContext* context);
	void CullSceneLights(ComponentRegistry* registry, RenderContext* context);

private:
	CullingPass();
	bool Init();

	FrustumUPtr m_frustum;
};