#pragma once
#include "Graphics/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(Framebuffer)
CLASS_PTR(PostProcessFramebuffer)
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
	virtual ~UniversalPostProcessPass();
	static UniversalPostProcessPassUPtr Create
	(
		int32 width = WINDOW_WIDTH,
		int32 heigh = WINDOW_HEIGHT
	);
	virtual void Render(RenderContext* context) override;
	void Resize(int32 width, int32 height);

	PostProcessFramebuffer* GetFramebuffer() const { return m_frameBuffer.get(); }

/*==============================================//
//   universal post-processing helper methods   //
//==============================================*/
private:
	Texture* RenderKawaseBloom(Texture* hdrTexture);
	void RenderComposite(Texture* hdrTexture, Texture* bloomTexture);

private:
	UniversalPostProcessPass();
	bool Init(int32 width, int32 height);

	ScreenMeshPtr			  m_plane;
	PostProcessFramebufferUPtr			  m_frameBuffer;
	ProgramUPtr				  m_bloomProgram;
	ProgramUPtr			      m_compositeProgram;
	float m_gamma			  { 2.2f };
	float m_exposure		  { 0.7f };
	float m_threshold		  { 1.0f };

	PostProcessFramebufferUPtr m_bloomFBO;				  // Bloom 처리용 FBO
	std::vector<KawaseBloomMips> m_bloomMips; // Bloom 텍스처 체인 (보통 5~6단계)
};