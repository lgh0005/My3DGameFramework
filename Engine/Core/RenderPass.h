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
	// virtual void Resize(int32 width, int32 height) {}

protected:
	RenderPass() = default;
	bool Init(ProgramUPtr program);
	ProgramUPtr m_program;
	std::vector<MeshRenderer*> m_renderers;
};

// TODO : 지금은 추상 클래스로 따로 두고 있지만 이들은 
// 동작 자체를 확정지은 렌더 패스로 만들어도 무방. 이후 아얘 확정된
// 동작을 하는 클래스로 아얘 만들어 둘지 고려 필요.
// 다만, 커스텀 렌더 패스들에 대한(Forward-Shading) 것들을 보장해줄
// 필요가 있음.

// 그전에 다중 조명이 잘 동작하는 지 부터 확인할 필요가 있음.
#pragma region ESSENTIAL_RENDER_PASSES
CLASS_PTR(ShadowPass)
class ShadowPass : public RenderPass
{
public:
	virtual ~ShadowPass();
	// virtual void CalculateLightSpaceMatrix(Scene* scene) = 0;
	TexturePtr GetDepthMap() const;
	ShadowMap* GetShadowMap(int32 index) { return m_shadowMaps[index].get(); }
	glm::mat4 GetLightSpaceMatrix() const;
	virtual void Resize(int32 width, int32 height) { }

protected:
	glm::mat4     m_lightSpaceMatrix;
	int32	      m_resolution;
	std::vector<ShadowMapUPtr> m_shadowMaps;
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
	Framebuffer* GetFramebuffer() const { return m_frameBuffer.get(); }
	void Resize(int32 width, int32 height);

	// DEBUG : IMGUI 테스트를 위한 포스트-프로세스 getter
	float* GetGammaPtr() { return &m_gamma; }
	float* GetExposurePtr() { return &m_exposure; }

protected:
	MeshPtr			m_plane;
	FramebufferUPtr m_frameBuffer;
	FramebufferUPtr m_pingPongFBOs[2];
	float m_gamma	  { 2.2f };
	float m_exposure  { 1.0f };
};

CLASS_PTR(GeometryPass)
class GeometryPass : public RenderPass
{
public:
	virtual ~GeometryPass();
	Framebuffer* GetGBuffer();
	void AddSkinnedMeshRenderer(MeshRenderer* skinnedMeshRenderer);
	const std::vector<MeshRenderer*>& GetSkinnedMeshRenderers() const;
	void Resize(int32 width, int32 height);

protected:
	FramebufferUPtr m_gBuffer;
	std::vector<MeshRenderer*> m_skinnedMeshRenderers;
};

CLASS_PTR(DeferredLightingPass)
class DeferredLightingPass : public RenderPass
{
public:
	virtual ~DeferredLightingPass();

protected:
	MeshPtr m_plane;
};
#pragma endregion