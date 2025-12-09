#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Camera)
#pragma endregion

CLASS_PTR(SphericalToCubePass)
class SphericalToCubePass : public GeneralRenderPass
{
public:
	static SphericalToCubePassUPtr Create(ProgramUPtr program);
	virtual ~SphericalToCubePass();

	virtual void Render(Scene* scene, Camera* camera) override;

	const std::vector<MeshRenderer*>& GetRenderers() const;
	void AddRenderer(MeshRenderer* meshRenderer);

private:
	SphericalToCubePass();
	bool Init(ProgramUPtr program);

	ProgramUPtr m_sphericalToCubeProgram;
	std::vector<MeshRenderer*> m_renderers;
};