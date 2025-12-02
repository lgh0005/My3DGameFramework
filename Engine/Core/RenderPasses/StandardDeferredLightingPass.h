#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(Mesh)
CLASS_PTR(Texture)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(StandardDeferredLightingPass)
class StandardDeferredLightingPass : public RenderPass
{
public:
	static StandardDeferredLightingPassUPtr Create();
	virtual void Render(Scene* scene, Camera* camera) override;
	void SetSSAOTexture(Texture* ssaoTex) { m_ssaoTexture = ssaoTex; }

private:
	StandardDeferredLightingPass() = default;
	bool Init();

	MeshPtr m_plane;
	ProgramUPtr m_deferredLightProgram;
	Texture* m_ssaoTexture{ nullptr };
};