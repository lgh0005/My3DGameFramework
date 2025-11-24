#include "EnginePch.h"
#include "RenderPass.h"
#include "Core/Scene.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Graphics/Program.h"
#include "Graphics/ShadowMap.h"
#include "Graphics/FrameBuffer.h"

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

/*=================================//
//   DeferredLightingPass methods  //
//=================================*/
DeferredLightingPass::~DeferredLightingPass() = default;