#pragma once
#include "Graphics/RenderPass.h"
#include "Graphics/Frustum.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
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
	void CullStaticMeshRenderers(Scene* scene, RenderContext* context);
	void CullSkinnedMeshRenderers(Scene* scene, RenderContext* context);
	void CullMeshOutlines(Scene* scene, RenderContext* context);
	void CullSceneLights(Scene* scene, RenderContext* context);

private:
	CullingPass();
	bool Init();

	FrustumUPtr m_frustum;
};