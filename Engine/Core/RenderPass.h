#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(MeshRenderer)
CLASS_PTR(Scene) 
CLASS_PTR(Camera)
#pragma endregion

CLASS_PTR(RenderPass)
class RenderPass
{
public:
	virtual ~RenderPass();
	void AddRenderer(MeshRenderer* meshRenderer);
	const std::vector<MeshRenderer*>& GetRenderers() const;
	Program& GetProgram() const { return *m_program; }
	virtual void Render(Scene* scene, Camera* camera) = 0;

protected:
	RenderPass() = default;
	bool Init(ProgramUPtr program);
	ProgramUPtr m_program;
	std::vector<MeshRenderer*> m_renderers;
};