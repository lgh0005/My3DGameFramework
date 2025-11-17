#pragma once
#include "Core/Renderer.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Framebuffer)
CLASS_PTR(CubeTexture)
CLASS_PTR(SpotLight)
CLASS_PTR(ShadowMap)
#pragma endregion

// TODO 
// 1. 프레임 버퍼 기반 렌더링

CLASS_PTR(DevRenderer)
class DevRenderer : public Renderer
{
public:
	static DevRendererUPtr Create(int32 width, int32 height);
	virtual void Render(Scene* scene) override;

private:
	DevRenderer() = default;
	virtual bool Init(int32 width, int32 height) override;

// TEMP : 역할에 맞게 우선 나눔
private:
	void RenderShadowPass(Scene* scene, Camera* camera, SpotLight* mainLight);
	void RenderMainPass(Scene* scene, Camera* camera, SpotLight* mainLight);
	void RenderSkyboxPass(Scene* scene, Camera* camera);
	void RenderPostProcessingPass(Scene* scene, Camera* camera);

private:
	// --- 후처리(Post-Processing) 리소스 ---
	FramebufferUPtr m_frameBuffer; 
	MeshUPtr		m_plane;       
	ProgramUPtr		m_postProgram; 
	float			m_gamma{ 1.0f };

	// --- 스카이 및 환경 박스 ---
	MeshUPtr		m_box;
	CubeTextureUPtr m_cubeTexture;
	ProgramUPtr		m_skyboxProgram;

	// --- 환경맵 리소스 ---
	ProgramUPtr m_envMapProgram;

	// --- 그림자 매핑 리소스 ---
	ShadowMapUPtr m_shadowMap;
	ProgramUPtr	  m_shadowDepthProgram;
	Light*		  m_mainLight;
};