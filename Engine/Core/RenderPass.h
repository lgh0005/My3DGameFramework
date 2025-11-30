#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(MeshRenderer)
CLASS_PTR(Scene) 
CLASS_PTR(Camera)
CLASS_PTR(ShadowMap)
CLASS_PTR(Framebuffer)
#pragma endregion

// TODO : 보다 범용적인 추상 클래스로 수정 필요
// 특히 Render에는 이제 더 이상 scene을 몰라도 수행될 수 있게
CLASS_PTR(RenderPass)
class RenderPass
{
public:
	virtual ~RenderPass();

	// TODO : Render의 메서드 구조를 고려할 필요가 있음
	virtual void Render(Scene* scene, Camera* camera) = 0;

	// TODO : 이것도 각각의 RenderPass가 따로 선언하도록 만들기
	const std::vector<MeshRenderer*>& GetRenderers() const;
	void AddRenderer(MeshRenderer* meshRenderer);
	// TODO : 이것도 각각의 RenderPass가 따로 선언하도록 만들기
	Program& GetProgram() const { return *m_program; }

protected:
	RenderPass() = default;
	bool Init(ProgramUPtr program); // TODO : 이 부분은 지워도 될듯
	ProgramUPtr m_program; // TODO : 이것도 각각의 RenderPass가 따로 선언하도록 만들기
	std::vector<MeshRenderer*> m_renderers; // TODO : 이것도 각각의 RenderPass가 따로
};