#include "EnginePch.h"
#include "StandardRenderPipeline.h"

#include "SRP/RenderPasses/StandardShadowPass.h"
#include "SRP/RenderPasses/StandardSkyboxPass.h"
#include "SRP/RenderPasses/StandardPostProcessPass.h"
#include "SRP/RenderPasses/StandardGeometryPass.h"
#include "SRP/RenderPasses/StandardSSAOPass.h"
#include "SRP/RenderPasses/StandardDeferredLightingPass.h"
#include "SRP/StandardGlobalUniforms.h"

#include "SRP/StandardRenderContext.h"


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

	// 0. 스택 영역에 StandardRenderContext 생성
	// TODO : 이후에는 이들이 CullingPass를 통해서 절두체 안에 있는 대상만 그림을 그리도록 만들어야 함
	// TODO : Reset에 들어갈 width와 height은 WindowManager를 통해서 얻어와야 함.
	StandardRenderContext context;
	context.Reset(scene, scene->GetMainCamera());

	// 1. ubo 갱신
	m_globalUniforms->PreRender(scene, camera);

	/*================================//
	//   main scene rendergin logic   //
	//================================*/

	// TODO : TestRenderer에다가 그냥 context에 있는 Renderer 또는 Lights를 
	// 넘겨줬을 뿐인데 왜 blur가 깨지는 지 전혀 모르겠다.
	// SOLUTION : 
	// -> 그러니까 AddComponent하면서 GameObject에 AddObject하면서 한 번,
	// lightPass에서 아마 관련 메쉬 렌더러를 또 추가하니까,
	// 디퍼드에서 한 번 렌더링, 포워드에서 또 중복 렌더링이 되는 것이 문제.
	// 오늘은 이걸 해결해보자.

	// 문제는 이렇게 하면 무조건적으로 AddObject시 디퍼드로 렌더링을 하겠다는 것이
	// 강제되버린다. 포워드 렌더링을 할 때는 Scene의 MeshRenderer에 들어가지 않도록
	// 보장해줄 필요가 있다.

	// [패스 1] 그림자 패스: m_shadowMap에 깊이 정보 기록
	// m_shadowPass->Render(scene, camera);
	m_shadowPass->TestRender(&context);

	// [패스 2] 디퍼드 셰이딩 및 포스트 프로세스 프레임 버퍼에 깊이 복사
	// m_geometryPass->Render(scene, camera);
	m_geometryPass->TestRender(&context);

	// [패스 3] SSAO
	{
		// 1. G-Buffer 가져오기
		auto gBuffer = m_geometryPass->GetGBuffer();
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
	// TODO : 아마 여기에서 blur 처리가 멋대로 되고 있을 것임.
	// 이는 blur와 관련된 layout을 꺼줄 필요가 있는 것이다. 이후 수정 필요.
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
	m_ssaoPass->Resize(width, height);
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