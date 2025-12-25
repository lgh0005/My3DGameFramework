#pragma once
#include "Core/RenderPass.h"

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
	static UniversalDeferredLightingPassUPtr Create();
	virtual void Render(RenderContext* context) override;
	void SetSSAOTexture(Texture* ssaoTex) { m_ssaoTexture = ssaoTex; }

private:
	UniversalDeferredLightingPass() = default;
	bool Init();

	ScreenMeshPtr m_plane;
	ProgramUPtr m_deferredLightProgram;
	Texture* m_ssaoTexture{ nullptr };
};