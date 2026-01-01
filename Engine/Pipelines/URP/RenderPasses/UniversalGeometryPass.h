#pragma once
#include "Graphics/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(Framebuffer)
CLASS_PTR(GBufferFramebuffer)
CLASS_PTR(RenderContext)
CLASS_PTR(StaticMeshRenderer)
CLASS_PTR(SkinnedMeshRenderer)
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
	void AddStaticMeshRenderer(StaticMeshRenderer* staticMeshRenderer);
	void AddSkinnedMeshRenderer(SkinnedMeshRenderer* skinnedMeshRenderer);
	const std::vector<StaticMeshRenderer*>& GetStaticMeshRenderers() const;
	const std::vector<SkinnedMeshRenderer*>& GetSkinnedMeshRenderers() const;
	GBufferFramebuffer* GetGBuffer() { return m_gBuffer.get(); }

	virtual void Render(RenderContext* context) override;

	void Resize(int32 width, int32 height);

private:
	UniversalGeometryPass();
	bool Init(int32 width, int32 height);
	void RenderStaticGeometry(const std::vector<StaticMeshRenderer*>& meshes);
	void RenderSkinnedGeometry(const std::vector<SkinnedMeshRenderer*>& meshes);

	GBufferFramebufferUPtr m_gBuffer;
	ProgramUPtr m_staticGeometryProgram;
	ProgramUPtr m_skinnedGeometryProgram;
	std::vector<StaticMeshRenderer*> m_staticMeshRenderers;
	std::vector<SkinnedMeshRenderer*> m_skinnedMeshRenderers;
};