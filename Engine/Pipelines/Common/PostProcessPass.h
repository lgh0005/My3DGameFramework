#pragma once
#include "Graphics/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(PostProcessFramebuffer)
CLASS_PTR(ScreenMesh)
CLASS_PTR(PostProcessEffect)
#pragma endregion

CLASS_PTR(PostProcessPass)
class PostProcessPass : public ContextRenderPass
{
public:
	virtual ~PostProcessPass();
	static PostProcessPassUPtr Create(int32 width = WINDOW_WIDTH, int32 height = WINDOW_HEIGHT);
	PostProcessFramebuffer* GetFramebuffer() const { return m_frameBuffer.get(); }
	virtual void Render(RenderContext* context) override;
	void PresentToScreen();

	void Resize(int32 width, int32 height);
	void AddEffect(PostProcessEffectUPtr effect);

private:
	PostProcessPass();
	bool Init(int32 width, int32 height);

	ScreenMeshPtr						  m_screen;
	PostProcessFramebufferUPtr			  m_frameBuffer;
	std::vector<PostProcessEffectUPtr>    m_effects;
};