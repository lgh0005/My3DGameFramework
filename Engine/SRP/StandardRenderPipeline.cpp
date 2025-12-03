#include "EnginePch.h"
#include "StandardRenderPipeline.h"

#include "SRP/RenderPasses/StandardShadowPass.h"
#include "SRP/RenderPasses/StandardSkyboxPass.h"
#include "SRP/RenderPasses/StandardPostProcessPass.h"
#include "SRP/RenderPasses/StandardGeometryPass.h"
#include "SRP/RenderPasses/StandardSSAOPass.h"
#include "SRP/RenderPasses/StandardDeferredLightingPass.h"
#include "SRP/StandardGlobalUniforms.h"

#include "Core/Scene.h"
#include "Core/GameObject.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h" 
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture.h"
#include "Graphics/CubeTexture.h"
#include "Graphics/ShadowMap.h"
#include "Components/Light.h"
#include "Components/SpotLight.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"
#include "Components/Animator.h"

StandardRenderPipelineUPtr StandardRenderPipeline::Create()
{
	auto renderPipeline = StandardRenderPipelineUPtr(new StandardRenderPipeline());
	if (!renderPipeline->Init()) return nullptr;
	return std::move(renderPipeline);
}

bool StandardRenderPipeline::Init()
{
	// 0. UBO 생성
	m_globalUniforms = StandardGlobalUniforms::Create();
	if (!m_globalUniforms) return false;

	// 1. 셰도우 패스 생성
	m_shadowPass = StandardShadowPass::Create();
	if (!m_shadowPass) return false;

	// 2. 스카이박스 패스 생성
	m_skyboxPass = StandardSkyboxPass::Create();
	if (!m_skyboxPass) return false;

	// 3. 포스트-프로세스 패스 생성
	m_postProcessPass = StandardPostProcessPass::Create();
	if (!m_postProcessPass) return false;

	// 4. G-buffer 패스 생성
	m_geometryPass = StandardGeometryPass::Create();
	if (!m_geometryPass) return false;

	// 5. Light 패스 생성
	m_deferredLightPass = StandardDeferredLightingPass::Create();
	if (!m_deferredLightPass) return false;

	// TEST : SSAO 패스 생성
	m_ssaoPass = StandardSSAOPass::Create();
	if (!m_ssaoPass) return false;

	return true;
}

void StandardRenderPipeline::BlitCopyDepth(Framebuffer* src, Framebuffer* dst,
										  int32 width,		 int32 height)
{
	if (!src || !dst) return;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, src->Get());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->Get());

	glBlitFramebuffer
	(
		0, 0, width, height,
		0, 0, width, height,
		GL_DEPTH_BUFFER_BIT,
		GL_NEAREST
	);
}

void StandardRenderPipeline::Render(Scene* scene)
{
	/*====================================//
	//   get essential scene properties   //
	//====================================*/
	// 메인 카메라 속성 가져오기
	auto* camera = scene->GetMainCamera();
	if (!camera) return;

	// 1. ubo 갱신
	m_globalUniforms->PreRender(scene, camera);

	/*================================//
	//   main scene rendergin logic   //
	//================================*/
	// [패스 1] 그림자 패스: m_shadowMap에 깊이 정보 기록
	m_shadowPass->Render(scene, camera);

	// [패스 2] 디퍼드 셰이딩 및 포스트 프로세스 프레임 버퍼에 깊이 복사
	m_geometryPass->Render(scene, camera);

	// [패스 3] SSAO
	{
		// 1. G-Buffer 가져오기
		auto gBuffer = m_geometryPass->GetGBuffer();

		// 2. [★핵심 수정★] SSAO 패스에 재료(텍스처) 넣어주기!
		// 이 부분이 빠져서 SSAO가 작동을 안 하고 있었던 겁니다.
		if (m_ssaoPass && gBuffer)
		{
			m_ssaoPass->SetGBufferInputs
			(
				gBuffer->GetColorAttachment(0).get(), // Position
				gBuffer->GetColorAttachment(1).get()  // Normal
			);
		}
		m_ssaoPass->Render(scene, camera);
	}

	// [패스 4] Deferred Lighting 패스
	m_deferredLightPass->SetSSAOTexture(m_ssaoPass->GetSSAOResultTexture());
	m_deferredLightPass->Render(scene, camera);

	auto gBuffer = m_geometryPass->GetGBuffer();
	auto postFBO = m_postProcessPass->GetFramebuffer();
	BlitCopyDepth(gBuffer, postFBO, gBuffer->GetWidth(), gBuffer->GetHeight());

	// [패스 3] 포워드 셰이딩

	for (const auto& [name, pass] : scene->GetCustomRenderPasses())
		pass->Render(scene, camera);

	// [패스 4] 스카이박스 패스: m_frameBuffer에 스카이박스 덧그리기
	m_skyboxPass->Render(scene, camera);

	// [패스 5] 후처리 패스: m_frameBuffer의 결과를 화면에 Resolve
	m_postProcessPass->Render(scene, camera);

	// [DEBUG] : ImGUI 컨텍스트
	RenderIMGUIContext();
}

void StandardRenderPipeline::OnResize(int32 width, int32 height)
{
	m_geometryPass->Resize(width, height);
	m_postProcessPass->Resize(width, height);
}

void StandardRenderPipeline::RenderIMGUIContext()
{
	IMGUI.BeginFrame();

	if (IMGUI.Begin("Test Window"))
	{
		ImGui::Text("Hello world!");
	} IMGUI.End();

	IMGUI.EndFrame();
}