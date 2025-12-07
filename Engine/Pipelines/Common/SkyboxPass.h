#pragma once
#include "Core/RenderPass.h"
#include "Graphics/Program.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(Mesh)
CLASS_PTR(CubeTexture)
CLASS_PTR(Program)
CLASS_PTR(RenderContext)
#pragma endregion

CLASS_PTR(SkyboxPass)
class SkyboxPass : public ContextRenderPass
{
public:
	static SkyboxPassUPtr Create();
	virtual void Render(RenderContext* context) override;

private:
	SkyboxPass() = default;
	bool Init();

	ProgramUPtr m_skyboxProgram;
	MeshUPtr m_cubeMesh;
};