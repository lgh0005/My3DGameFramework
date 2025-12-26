#pragma once
#include "Core/RenderPass.h"

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
CLASS_PTR(UniversalRenderContext)
#pragma endregion

CLASS_PTR(UniversalSSAOPass)
class UniversalSSAOPass : public ContextRenderPass
{
public:
	virtual ~UniversalSSAOPass();
	static UniversalSSAOPassUPtr Create
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
	void ComputeSSAO(UniversalRenderContext* context);
	void BlurSSAOResult(UniversalRenderContext* context);
	void GenerateKernel();
	void GenerateNoiseTexture();

private:
	UniversalSSAOPass();
	bool Init(int32 width, int32 height);

	ScreenMeshUPtr		   m_screenQuad{ nullptr };

	std::vector<glm::vec3> m_ssaoKernel;
	TextureUPtr            m_noiseTexture{ nullptr };

	SSAOFramebufferUPtr        m_ssaoFBO{ nullptr };	 // Raw SSAO
	SSAOFramebufferUPtr        m_ssaoBlurFBO{ nullptr }; // Blurred SSAO

	ProgramUPtr            m_ssaoProgram{ nullptr };
	ProgramUPtr            m_ssaoBlurProgram{ nullptr };
};