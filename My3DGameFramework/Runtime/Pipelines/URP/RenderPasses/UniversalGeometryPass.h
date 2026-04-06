#pragma once
#include "Graphics/Rendering/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Framebuffer)
CLASS_PTR(GBufferFramebuffer)
CLASS_PTR(RenderContext)
CLASS_PTR(StaticMeshRenderer)
CLASS_PTR(SkinnedMeshRenderer)
CLASS_PTR(RenderQueue)
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
	GBufferFramebuffer* GetGBuffer() { return m_gBuffer.get(); }

	virtual void Render(RenderContext* context) override;

	void Resize(int32 width, int32 height);

private:
	UniversalGeometryPass();
	bool Init(int32 width, int32 height);
	void RenderGeometry(RenderContext* context, const glm::mat4& lightSpaceMatrix);

	ProgramPtr m_geometryInstancedProgram;
	GBufferFramebufferUPtr m_gBuffer;
	RenderQueueUPtr m_renderQueue;
};