#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Camera)
#pragma endregion

CLASS_PTR(TestRenderPass)
class TestRenderPass : public GeneralRenderPass
{
public:
	static TestRenderPassUPtr Create(ProgramUPtr program);
	virtual void Render(Scene* scene, Camera* camera) override;

	const std::vector<MeshRenderer*>& GetRenderers() const;
	void AddRenderer(MeshRenderer* meshRenderer);

private:
	TestRenderPass() = default;
	bool Init(ProgramUPtr program);
	ProgramUPtr m_simpleProgram;
	std::vector<MeshRenderer*> m_renderers;
};