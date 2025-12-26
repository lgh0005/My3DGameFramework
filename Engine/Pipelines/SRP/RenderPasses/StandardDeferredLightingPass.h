#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(ScreenMesh)
CLASS_PTR(Texture)
CLASS_PTR(Program)
CLASS_PTR(RenderContext)
CLASS_PTR(StandardRenderContext)
#pragma endregion

CLASS_PTR(StandardDeferredLightingPass)
class StandardDeferredLightingPass : public ContextRenderPass
{
public:
	static StandardDeferredLightingPassUPtr Create();
	virtual void Render(RenderContext* context) override;
	void SetSSAOTexture(Texture* ssaoTex) { m_ssaoTexture = ssaoTex; }

/*==========================================//
//   deferred lighting pass helper methods  //
//==========================================*/
private:
	void BeginDrawOnPostProcessFBO(StandardRenderContext* context);
	void BindSSAOTexture(StandardRenderContext* context);
	void BindShadowMaps(StandardRenderContext* context);
	void GetLightMatricesFromContext(StandardRenderContext* context);

private:
	StandardDeferredLightingPass() = default;
	bool Init();

	ScreenMeshPtr m_plane;
	ProgramUPtr m_deferredLightProgram;
	Texture* m_ssaoTexture{ nullptr };
};