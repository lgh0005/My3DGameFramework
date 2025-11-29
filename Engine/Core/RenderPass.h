#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(MeshRenderer)
CLASS_PTR(Scene) 
CLASS_PTR(Camera)
CLASS_PTR(ShadowMap)
CLASS_PTR(Framebuffer)
#pragma endregion

// TODO : 보다 범용적인 추상 클래스로 수정 필요
// 특히 Render에는 이제 더 이상 scene을 몰라도 수행될 수 있게
CLASS_PTR(RenderPass)
class RenderPass
{
public:
	virtual ~RenderPass();

	// TODO : Render의 메서드 구조를 고려할 필요가 있음
	virtual void Render(Scene* scene, Camera* camera) = 0;

	// TODO : 이것도 각각의 RenderPass가 따로 선언하도록 만들기
	const std::vector<MeshRenderer*>& GetRenderers() const;
	void AddRenderer(MeshRenderer* meshRenderer);
	// TODO : 이것도 각각의 RenderPass가 따로 선언하도록 만들기
	Program& GetProgram() const { return *m_program; }

protected:
	RenderPass() = default;
	bool Init(ProgramUPtr program); // TODO : 이 부분은 지워도 될듯
	ProgramUPtr m_program; // TODO : 이것도 각각의 RenderPass가 따로 선언하도록 만들기
	std::vector<MeshRenderer*> m_renderers; // TODO : 이것도 각각의 RenderPass가 따로
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
	ShadowMap* GetShadowMap(int32 index) { return m_shadowMaps[index].get(); }
	virtual void Resize(int32 width, int32 height) { }

protected:
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