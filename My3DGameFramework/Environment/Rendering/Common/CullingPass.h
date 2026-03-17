#pragma once
#include "Graphics/Rendering/RenderPass.h"
#include "Graphics/Geometry/Frustum.h"

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
	void CullStaticMeshRenderers(RenderContext* context);
	void CullSkinnedMeshRenderers(RenderContext* context);
	void CullMeshOutlines(RenderContext* context);
	void CullSceneLights(RenderContext* context);

private:
	CullingPass();
	bool Init();

	FrustumUPtr m_frustum;
};