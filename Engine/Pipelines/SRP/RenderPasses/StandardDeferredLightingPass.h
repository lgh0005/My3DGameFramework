#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(Mesh)
CLASS_PTR(Texture)
CLASS_PTR(Program)
CLASS_PTR(RenderContext)
#pragma endregion

CLASS_PTR(StandardDeferredLightingPass)
class StandardDeferredLightingPass : public ContextRenderPass
{
public:
	static StandardDeferredLightingPassUPtr Create();
	virtual void Render(RenderContext* context) override;
	void SetSSAOTexture(Texture* ssaoTex) { m_ssaoTexture = ssaoTex; }

private:
	StandardDeferredLightingPass() = default;
	bool Init();

	MeshPtr m_plane;
	ProgramUPtr m_deferredLightProgram;
	Texture* m_ssaoTexture{ nullptr };
};