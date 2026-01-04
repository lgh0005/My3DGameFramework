#pragma once
#include "Graphics/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(Program)
CLASS_PTR(RenderContext)
#pragma endregion

CLASS_PTR(SkyboxPass)
class SkyboxPass : public ContextRenderPass
{
public:
	virtual ~SkyboxPass();
	static SkyboxPassUPtr Create();
	virtual void Render(RenderContext* context) override;

private:
	SkyboxPass();
	bool Init();

	ProgramPtr m_skyboxProgram;
	MeshPtr m_cubeMesh;
};