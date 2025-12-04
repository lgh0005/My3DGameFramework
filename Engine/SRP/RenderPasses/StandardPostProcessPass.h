#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(Framebuffer)
CLASS_PTR(RenderContext)
#pragma endregion

CLASS_PTR(StandardPostProcessPass)
class StandardPostProcessPass : public RenderPass
{
public:
	static StandardPostProcessPassUPtr Create
	(
		int32 width = WINDOW_WIDTH,
		int32 heigh = WINDOW_HEIGHT
	);
	virtual void Render(Scene* scene, Camera* camera) override;
	
	void TestRender(RenderContext* context);
	
	void BeginDraw();
	void Resize(int32 width, int32 height);

	Framebuffer* GetFramebuffer() const { return m_frameBuffer.get(); }

private:
	StandardPostProcessPass() = default;
	bool Init(int32 width, int32 height);

	MeshPtr			m_plane;
	ProgramUPtr		m_blurProgram;
	ProgramUPtr		m_compositeProgram;
	FramebufferUPtr m_frameBuffer;
	FramebufferUPtr m_pingPongFBOs[2];
	float m_gamma			 { 2.2f };
	float m_exposure		 { 1.0f };
};