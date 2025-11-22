#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(MeshRenderer)
CLASS_PTR(Scene) 
CLASS_PTR(Camera)
CLASS_PTR(ShadowMap)
CLASS_PTR(Framebuffer)
#pragma endregion

CLASS_PTR(RenderPass)
class RenderPass
{
public:
	virtual ~RenderPass();
	void AddRenderer(MeshRenderer* meshRenderer);
	const std::vector<MeshRenderer*>& GetRenderers() const;
	Program& GetProgram() const { return *m_program; }
	virtual void Render(Scene* scene, Camera* camera) = 0;

protected:
	RenderPass() = default;
	bool Init(ProgramUPtr program);
	ProgramUPtr m_program;
	std::vector<MeshRenderer*> m_renderers;
};

CLASS_PTR(ShadowPass)
class ShadowPass : public RenderPass
{
public:
	virtual ~ShadowPass();
	virtual void CalculateLightSpaceMatrix(Scene* scene) = 0;
	TexturePtr GetDepthMap() const;
	glm::mat4 GetLightSpaceMatrix() const;

protected:
	glm::mat4     m_lightSpaceMatrix;
	int32	      m_resolution;
	ShadowMapUPtr m_shadowMap;
};

CLASS_PTR(SkyboxPass)
class SkyboxPass : public RenderPass
{
public:
	virtual ~SkyboxPass();

protected:
	MeshPtr m_cubeMesh;
	CubeTexturePtr m_cubeTexture;
};

CLASS_PTR(PostProcessPass)
class PostProcessPass : public RenderPass
{
public:
	virtual ~PostProcessPass();
	void BeginDraw();

protected:
	MeshPtr			m_plane;
	FramebufferUPtr m_frameBuffer;
	float m_gamma{ 2.2f };
	float m_exposure{ 1.0f };
};