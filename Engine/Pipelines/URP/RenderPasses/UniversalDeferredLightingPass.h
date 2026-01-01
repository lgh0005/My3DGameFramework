#pragma once
#include "Graphics/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(ScreenMesh)
CLASS_PTR(Texture)
CLASS_PTR(Program)
CLASS_PTR(RenderContext)
CLASS_PTR(UniversalRenderContext)
#pragma endregion

CLASS_PTR(UniversalDeferredLightingPass)
class UniversalDeferredLightingPass : public ContextRenderPass
{
public:
	virtual ~UniversalDeferredLightingPass();
	static UniversalDeferredLightingPassUPtr Create();
	virtual void Render(RenderContext* context) override;
	void SetSSAOTexture(Texture* ssaoTex) { m_ssaoTexture = ssaoTex; }

/*===========================================//
//   deferred lighting pass helper methods   //
//===========================================*/
private:
	void BeginDrawOnPostProcessFBO(UniversalRenderContext* context);
	void BindSSAOTexture(UniversalRenderContext* context);
	void BindShadowMaps(UniversalRenderContext* context);
	void BindIBLMaps(UniversalRenderContext* context);
	void GetLightMatricesFromContext(UniversalRenderContext* context);

private:
	UniversalDeferredLightingPass();
	bool Init();

	ScreenMeshPtr m_plane;
	ProgramUPtr m_deferredLightProgram;
	Texture* m_ssaoTexture{ nullptr };
};