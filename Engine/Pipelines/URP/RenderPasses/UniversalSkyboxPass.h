#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(Program)
CLASS_PTR(RenderContext)
#pragma endregion

CLASS_PTR(UniversalSkyboxPass)
class UniversalSkyboxPass : public ContextRenderPass
{
public:
	static UniversalSkyboxPassUPtr Create();
	virtual void Render(RenderContext* context) override;

private:
	UniversalSkyboxPass() = default;
	bool Init();

	ProgramUPtr m_hdrSkyboxProgram;
	MeshUPtr    m_cubeMesh;
};