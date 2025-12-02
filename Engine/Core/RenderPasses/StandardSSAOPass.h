#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(Texture)
CLASS_PTR(Framebuffer)
CLASS_PTR(Program)
CLASS_PTR(StaticMesh)
#pragma endregion

CLASS_PTR(StandardSSAOPass)
class StandardSSAOPass : public RenderPass
{
public:
	static StandardSSAOPassUPtr Create
	(
		int32 width = WINDOW_WIDTH, 
		int32 height = WINDOW_HEIGHT
	);
	virtual void Render(Scene* scene, Camera* camera) override;
	void Resize(int32 width, int32 height);

	void SetGBufferInputs(Texture* positionTex, Texture* normalTex)
	{
		m_gPosition = positionTex;
		m_gNormal = normalTex;
	}

	Texture* GetSSAOResultTexture() const;

private:
	StandardSSAOPass() = default;
	bool Init(int32 width, int32 height);
	void GenerateKernel();
	void GenerateNoiseTexture();

	StaticMeshUPtr m_screenQuad{ nullptr };

	std::vector<glm::vec3> m_ssaoKernel;
	TextureUPtr            m_noiseTexture{ nullptr };

	FramebufferUPtr        m_ssaoFBO{ nullptr }; // Raw SSAO
	FramebufferUPtr        m_ssaoBlurFBO{ nullptr }; // Blurred SSAO

	ProgramUPtr            m_ssaoProgram{ nullptr };
	ProgramUPtr            m_ssaoBlurProgram{ nullptr };

	Texture* m_gPosition{ nullptr };
	Texture* m_gNormal{ nullptr };
};