#pragma once
#include "Graphics/Rendering/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(ScreenMesh)
CLASS_PTR(Texture)
CLASS_PTR(Program)
CLASS_PTR(RenderContext)
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
	void BeginDrawOnPostProcessFBO(RenderContext* context);
	void BindSSAOTexture(RenderContext* context);
	void BindShadowMaps(RenderContext* context);
	void BindIBLMaps(RenderContext* context);
	void GetLightMatricesFromContext(RenderContext* context);

private:
	UniversalDeferredLightingPass();
	bool Init();

	ScreenMeshPtr m_plane;
	ProgramPtr m_deferredLightProgram;
	Texture* m_ssaoTexture{ nullptr };
};