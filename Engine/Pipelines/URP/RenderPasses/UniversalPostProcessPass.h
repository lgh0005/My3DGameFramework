#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(Framebuffer)
CLASS_PTR(ScreenMesh)
CLASS_PTR(RenderContext)
CLASS_PTR(Program)
CLASS_PTR(Texture)
#pragma endregion

struct KawaseBloomMips
{
	TextureUPtr texture;
	int32 width; int32 height;
};

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
	ProgramUPtr				  m_bloomProgram;
	ProgramUPtr			      m_compositeProgram;
	float m_gamma			  { 2.2f };
	float m_exposure		  { 0.7f };
	float m_threshold		  { 1.0f };

	std::vector<KawaseBloomMips> m_bloomMips; // Bloom 텍스처 체인 (보통 5~6단계)
	FramebufferUPtr m_bloomFBO;				  // Bloom 처리용 FBO
};