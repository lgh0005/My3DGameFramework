#pragma once
#include "Graphics/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(ScreenMesh)
CLASS_PTR(Texture)
CLASS_PTR(Program)
CLASS_PTR(RenderContext)
#pragma endregion

CLASS_PTR(StandardDeferredLightingPass)
class StandardDeferredLightingPass : public ContextRenderPass
{
public:
	virtual ~StandardDeferredLightingPass();
	static StandardDeferredLightingPassUPtr Create();
	virtual void Render(RenderContext* context) override;

/*==========================================//
//   deferred lighting pass helper methods  //
//==========================================*/
private:
	void BeginDrawOnPostProcessFBO(RenderContext* context);
	void BindSSAOTexture(RenderContext* context);
	void BindShadowMaps(RenderContext* context);
	void GetLightMatricesFromContext(RenderContext* context);

private:
	StandardDeferredLightingPass();
	bool Init();

	ScreenMeshPtr m_plane;
	ProgramPtr m_deferredLightProgram;
};