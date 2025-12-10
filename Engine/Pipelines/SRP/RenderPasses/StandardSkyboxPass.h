#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(Program)
CLASS_PTR(RenderContext)
#pragma endregion

CLASS_PTR(StandardSkyboxPass)
class StandardSkyboxPass : public ContextRenderPass
{
public:
	static StandardSkyboxPassUPtr Create();
	virtual void Render(RenderContext* context) override;

private:
	StandardSkyboxPass() = default;
	bool Init();

	ProgramUPtr m_skyboxProgram;
	MeshUPtr m_cubeMesh;
};