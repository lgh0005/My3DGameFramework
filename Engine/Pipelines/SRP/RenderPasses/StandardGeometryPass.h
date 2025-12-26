#pragma once
#include "Core/RenderPass.h"
#include "Graphics/Program.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Framebuffer)
CLASS_PTR(RenderContext)
CLASS_PTR(GBufferFramebuffer)
#pragma endregion

CLASS_PTR(StandardGeometryPass)
class StandardGeometryPass : public ContextRenderPass
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
	GBufferFramebuffer* GetGBuffer() { return m_gBuffer.get(); }

	virtual void Render(RenderContext* context) override;
	void Resize(int32 width, int32 height);

private:
	StandardGeometryPass() = default;
	bool Init(int32 width, int32 height);
	void RenderStaticGeometry(const std::vector<MeshRenderer*>& meshes);
	void RenderSkinnedGeometry(const std::vector<MeshRenderer*>& meshes);

	GBufferFramebufferUPtr m_gBuffer;
	ProgramUPtr m_staticGeometryProgram;
	ProgramUPtr m_skinnedGeometryProgram;
	std::vector<MeshRenderer*> m_skinnedMeshRenderers;
	std::vector<MeshRenderer*> m_staticMeshRenderers;
};