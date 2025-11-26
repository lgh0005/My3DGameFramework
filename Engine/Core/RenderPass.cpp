#include "EnginePch.h"
#include "RenderPass.h"
#include "Core/Scene.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Graphics/Program.h"
#include "Graphics/ShadowMap.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture.h"

/*=======================//
//   RenderPass methods  //
//=======================*/
RenderPass::~RenderPass() = default;

bool RenderPass::Init(ProgramUPtr program)
{
	m_program = std::move(program);
	if (!m_program) return false;
	return true;
}

void RenderPass::AddRenderer(MeshRenderer* renderer)
{
	m_renderers.push_back(renderer);
}

const std::vector<MeshRenderer*>& RenderPass::GetRenderers() const
{
	return m_renderers;
}

/*=======================//
//   ShadowPass methods  //
//=======================*/
ShadowPass::~ShadowPass() = default;

TexturePtr ShadowPass::GetDepthMap() const
{
	return m_shadowMap->GetShadowMap();
}

glm::mat4 ShadowPass::GetLightSpaceMatrix() const
{
	return m_lightSpaceMatrix;
}

/*===================//
//   Skybox methods  //
//===================*/
SkyboxPass::~SkyboxPass() = default;


/*============================//
//   PostProcessPass methods  //
//============================*/
PostProcessPass::~PostProcessPass() = default;

void PostProcessPass::BeginDraw()
{
	m_frameBuffer->Bind();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

// TEMP : 윈도우 리사이즈 대응 테스트
void PostProcessPass::Resize(int32 width, int32 height)
{
	m_frameBuffer = Framebuffer::Create(width, height, 1);
	m_pingPongFBOs[0] = Framebuffer::Create(width, height, 1);
	m_pingPongFBOs[1] = Framebuffer::Create(width, height, 1);

	if (m_program)
	{
		m_program->Use();
		m_program->SetUniform("inverseScreenSize",
			glm::vec2(1.0f / (float)width, 1.0f / (float)height));
	}
}

/*=========================//
//   GeometryPass methods  //
//=========================*/
GeometryPass::~GeometryPass() = default;

void GeometryPass::AddSkinnedMeshRenderer(MeshRenderer* skinnedMeshRenderer)
{
	m_skinnedMeshRenderers.push_back(skinnedMeshRenderer);
}

const std::vector<MeshRenderer*>& GeometryPass::GetSkinnedMeshRenderers() const
{
	return m_skinnedMeshRenderers;
}

Framebuffer* GeometryPass::GetGBuffer()
{
	return m_gBuffer.get();
}

// TEMP : 윈도우 리사이즈 대응 테스트
void GeometryPass::Resize(int32 width, int32 height)
{
	m_gBuffer = Framebuffer::CreateGBuffer(width, height);
}

/*=================================//
//   DeferredLightingPass methods  //
//=================================*/
DeferredLightingPass::~DeferredLightingPass() = default;