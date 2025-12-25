#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(Framebuffer)
CLASS_PTR(RenderContext)
#pragma endregion

CLASS_PTR(UniversalGeometryPass)
class UniversalGeometryPass : public ContextRenderPass
{
public:
	virtual ~UniversalGeometryPass();
	static UniversalGeometryPassUPtr Create
	(
		int32 width = WINDOW_WIDTH,
		int32 heigh = WINDOW_HEIGHT
	);
	void AddStaticMeshRenderer(MeshRenderer* staticMeshRenderer);
	void AddSkinnedMeshRenderer(MeshRenderer* skinnedMeshRenderer);
	const std::vector<MeshRenderer*>& GetSkinnedMeshRenderers() const;
	const std::vector<MeshRenderer*>& GetStaticMeshRenderers() const;
	Framebuffer* GetGBuffer() { return m_gBuffer.get(); }

	virtual void Render(RenderContext* context) override;

	void Resize(int32 width, int32 height);

private:
	UniversalGeometryPass();
	bool Init(int32 width, int32 height);

	FramebufferUPtr m_gBuffer;
	ProgramUPtr m_staticGeometryProgram;
	ProgramUPtr m_skinnedGeometryProgram;
	std::vector<MeshRenderer*> m_skinnedMeshRenderers;
	std::vector<MeshRenderer*> m_staticMeshRenderers;
};