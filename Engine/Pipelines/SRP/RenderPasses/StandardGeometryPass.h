#pragma once
#include "Graphics/RenderPass.h"
#include "Resources/Program.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Framebuffer)
CLASS_PTR(RenderContext)
CLASS_PTR(GBufferFramebuffer)
CLASS_PTR(StaticMeshRenderer)
CLASS_PTR(SkinnedMeshRenderer)
#pragma endregion

CLASS_PTR(StandardGeometryPass)
class StandardGeometryPass : public ContextRenderPass
{
public:
	virtual ~StandardGeometryPass();
	static StandardGeometryPassUPtr Create
	(
		int32 width = WINDOW_WIDTH,
		int32 heigh = WINDOW_HEIGHT
	);
	GBufferFramebuffer* GetGBuffer() { return m_gBuffer.get(); }

	virtual void Render(RenderContext* context) override;
	void Resize(int32 width, int32 height);

private:
	StandardGeometryPass();
	bool Init(int32 width, int32 height);
	void RenderStaticGeometry(const std::vector<StaticMeshRenderer*>& meshes, const glm::mat4& vp);
	void RenderSkinnedGeometry(const std::vector<SkinnedMeshRenderer*>& meshes, const glm::mat4& vp);

	GBufferFramebufferUPtr m_gBuffer;
	ProgramPtr m_staticGeometryProgram;
	ProgramPtr m_skinnedGeometryProgram;
};