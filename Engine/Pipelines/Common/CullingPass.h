#pragma once
#include "Core/RenderPass.h"
#include "Graphics/Frustum.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(StandardRenderContext)
#pragma endregion

CLASS_PTR(CullingPass)
class CullingPass : public ContextRenderPass
{
public:
	static CullingPassUPtr Create();
	virtual ~CullingPass();

	virtual void Render(RenderContext* context) override;

/*=====================//
//   culling methods   //
//=====================*/
private:
	void CullStaticMeshRenderers(Scene* scene, StandardRenderContext* context);
	void CullSkinnedMeshRenderers(Scene* scene, StandardRenderContext* context);
	void CullSceneLights(Scene* scene, StandardRenderContext* context);

private:
	CullingPass();
	bool Init();

	FrustumUPtr m_frustum;
};