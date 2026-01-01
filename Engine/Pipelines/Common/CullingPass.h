#pragma once
#include "Graphics/RenderPass.h"
#include "Graphics/Frustum.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(SceneRegistry)
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
	void CullStaticMeshRenderers(SceneRegistry* registry, RenderContext* context);
	void CullSkinnedMeshRenderers(SceneRegistry* registry, RenderContext* context);
	void CullMeshOutlines(SceneRegistry* registry, RenderContext* context);
	void CullSceneLights(SceneRegistry* registry, RenderContext* context);

private:
	CullingPass();
	bool Init();

	FrustumUPtr m_frustum;
};