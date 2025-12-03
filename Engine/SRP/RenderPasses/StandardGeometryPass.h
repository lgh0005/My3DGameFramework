#pragma once
#include "Core/RenderPass.h"
#include "Graphics/Program.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(Framebuffer)
CLASS_PTR(RenderContext)
#pragma endregion

CLASS_PTR(StandardGeometryPass)
class StandardGeometryPass : public RenderPass
{
public:
	static StandardGeometryPassUPtr Create
	(
		int32 width = WINDOW_WIDTH,
		int32 heigh = WINDOW_HEIGHT
	);

	void AddStaticMeshRenderer(MeshRenderer* staticMeshRenderer);
	void AddSkinnedMeshRenderer(MeshRenderer* skinnedMeshRenderer);
	const std::vector<MeshRenderer*>& GetSkinnedMeshRenderers() const;
	const std::vector<MeshRenderer*>& GetStaticMeshRenderers() const;
	Framebuffer* GetGBuffer() { return m_gBuffer.get(); }

	virtual void Render(Scene* scene, Camera* camera) override;

	// TEST : Context에 있는 내용물을 잘 렌더링 하는 지 테스트
	void TestRender(RenderContext* context);

	void Resize(int32 width, int32 height);

private:
	StandardGeometryPass() = default;
	bool Init(int32 width, int32 height);

	FramebufferUPtr m_gBuffer;
	ProgramUPtr m_staticGeometryProgram;
	ProgramUPtr m_skinnedGeometryProgram;
	std::vector<MeshRenderer*> m_skinnedMeshRenderers;
	std::vector<MeshRenderer*> m_staticMeshRenderers;
};