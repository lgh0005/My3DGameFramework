#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(Framebuffer)
CLASS_PTR(ScreenMesh)
CLASS_PTR(RenderContext)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(UniversalPostProcessPass)
class UniversalPostProcessPass : public ContextRenderPass
{
public:
	static UniversalPostProcessPassUPtr Create
	(
		int32 width = WINDOW_WIDTH,
		int32 heigh = WINDOW_HEIGHT
	);
	virtual void Render(RenderContext* context) override;
	void Resize(int32 width, int32 height);

	Framebuffer* GetFramebuffer() const { return m_frameBuffer.get(); }

private:
	UniversalPostProcessPass() = default;
	bool Init(int32 width, int32 height);

	ScreenMeshPtr			  m_plane;
	FramebufferUPtr			  m_frameBuffer;
	ProgramUPtr			      m_toneMappingProgram;
	float m_gamma{ 2.2f };
	float m_exposure{ 1.0f };
};