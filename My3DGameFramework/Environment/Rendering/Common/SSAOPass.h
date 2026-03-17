#pragma once
#include "Graphics/Rendering/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(Texture)
CLASS_PTR(Framebuffer)
CLASS_PTR(SSAOFramebuffer)
CLASS_PTR(Program)
CLASS_PTR(ScreenMesh)
CLASS_PTR(StaticMesh)
CLASS_PTR(RenderContext)
CLASS_PTR(StandardRenderContext)
#pragma endregion

CLASS_PTR(SSAOPass)
class SSAOPass : public ContextRenderPass
{
public:
	virtual ~SSAOPass();
	static SSAOPassUPtr Create
	(
		int32 width = WINDOW_WIDTH, 
		int32 height = WINDOW_HEIGHT
	);
	virtual void Render(RenderContext* context) override;
	void Resize(int32 width, int32 height);
	
/*==============================//
//   ssao pass helper methods   //
//==============================*/
private:
	void ComputeSSAO(RenderContext* context);
	void BlurSSAOResult(RenderContext* context);
	void GenerateKernel();
	void GenerateNoiseTexture();

private:
	SSAOPass();
	bool Init(int32 width, int32 height);

	ScreenMeshPtr		   m_screenQuad{ nullptr };
	ProgramPtr            m_ssaoProgram{ nullptr };
	ProgramPtr            m_ssaoBlurProgram{ nullptr };

	std::vector<glm::vec3> m_ssaoKernel;
	TexturePtr            m_noiseTexture{ nullptr };

	SSAOFramebufferUPtr    m_ssaoFBO{ nullptr };	 // Raw SSAO
	SSAOFramebufferUPtr    m_ssaoBlurFBO{ nullptr }; // Blurred SSAO
};