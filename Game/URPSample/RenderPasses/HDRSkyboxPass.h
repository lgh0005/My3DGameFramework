#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Camera)
#pragma endregion

CLASS_PTR(HDRSkyboxPass)
class HDRSkyboxPass : public GeneralRenderPass
{
public:
	static HDRSkyboxPassUPtr Create(ProgramUPtr program);
	virtual void Render(Scene* scene, Camera* camera) override;

private:
	HDRSkyboxPass() = default;
	bool Init(ProgramUPtr program);

	ProgramUPtr m_skyboxProgram;
	MeshUPtr m_cubeMesh;
};