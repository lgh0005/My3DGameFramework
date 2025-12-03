#pragma once
#include "Core/RenderPass.h"

#include "Graphics/Program.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(Mesh)
CLASS_PTR(CubeTexture)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(StandardSkyboxPass)
class StandardSkyboxPass : public RenderPass
{
public:
	static StandardSkyboxPassUPtr Create();
	virtual void Render(Scene* scene, Camera* camera) override;

private:
	StandardSkyboxPass() = default;
	bool Init();

	ProgramUPtr m_skyboxProgram;
	MeshUPtr m_cubeMesh;
};