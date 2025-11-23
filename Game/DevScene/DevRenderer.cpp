#include "pch.h"
#include "DevRenderer.h"

#include "Core/Scene.h"
#include "Core/RenderPass.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Components/Animator.h"
#include "Core/GameObject.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h"
#include "Graphics/SkinnedMesh.h"
#include "Graphics/InstancedMesh.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture.h"
#include "Graphics/Material.h"
#include "Graphics/CubeTexture.h"
#include "Graphics/Image.h"
#include "Graphics/ShadowMap.h"

#include "RenderPasses/PostProcessingPass.h"

DevRendererUPtr DevRenderer::Create(int32 width, int32 height)
{
	auto devRenderer = DevRendererUPtr(new DevRenderer());
	return std::move(devRenderer);
}

// TODO : 이후에는 Deferred Shading 로직에 맞는 코드로 전면 수정 필요
// 현재는 다른 그래픽스 요소들을 학습 및 구현을 위해서 Forward Shading으로 진행
void DevRenderer::Render(Scene* scene)
{
	// 메인 카메라 속성 가져오기
	auto* camera = scene->GetMainCamera();
	if (!camera) return;

	// 메인 조명 속성 가져오기
	SpotLight* mainLight = static_cast<SpotLight*>(scene->GetMainLight());

	// [패스 1] 그림자 패스: m_shadowMap에 깊이 정보 기록
	scene->GetShadowPass()->Render(scene, camera);

	// [패스 2] 포워드 셰이딩 
	// RenderMainPass(scene, camera, mainLight);

	// TARGET : [패스 2] 디퍼스 셰이딩 
	scene->GetGeometryPass()->Render(scene, camera);
	scene->GetDeferredLightingPass()->Render(scene, camera);

	// 디버그 코드
	{
		//auto gBuffer = scene->GetGeometryPass()->GetGBuffer();

		//// 1. 읽어올 버퍼(G-Buffer)와 그릴 버퍼(화면) 바인딩
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->Get());
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // 0번은 기본 화면(Back Buffer)

		//// 화면 해상도 가져오기 (전역 변수 혹은 멤버 변수 사용)
		//int w = WINDOW_WIDTH;
		//int h = WINDOW_HEIGHT;
		//int halfW = w / 2;
		//int halfH = h / 2;

		//// -------------------------------------------------------
		//// [좌상단] Position (Attachment 0)
		//// -------------------------------------------------------
		//glReadBuffer(GL_COLOR_ATTACHMENT0);
		//glBlitFramebuffer(
		//	0, 0, w, h,                 // 소스 영역 (전체)
		//	0, halfH, halfW, h,         // 타겟 영역 (좌상단)
		//	GL_COLOR_BUFFER_BIT, GL_NEAREST
		//);

		//// -------------------------------------------------------
		//// [우상단] Normal (Attachment 1)
		//// -------------------------------------------------------
		//glReadBuffer(GL_COLOR_ATTACHMENT1);
		//glBlitFramebuffer(
		//	0, 0, w, h,                 // 소스 영역
		//	halfW, halfH, w, h,         // 타겟 영역 (우상단)
		//	GL_COLOR_BUFFER_BIT, GL_NEAREST
		//);

		//// -------------------------------------------------------
		//// [좌하단] Albedo + Specular (Attachment 2)
		//// -------------------------------------------------------
		//glReadBuffer(GL_COLOR_ATTACHMENT2);
		//glBlitFramebuffer(
		//	0, 0, w, h,                 // 소스 영역
		//	0, 0, halfW, halfH,         // 타겟 영역 (좌하단)
		//	GL_COLOR_BUFFER_BIT, GL_NEAREST
		//);

		//// -------------------------------------------------------
		//// [우하단] Emission (Attachment 3)
		//// -------------------------------------------------------
		//glReadBuffer(GL_COLOR_ATTACHMENT3);
		//glBlitFramebuffer(
		//	0, 0, w, h,                 // 소스 영역
		//	halfW, 0, w, halfH,         // 타겟 영역 (우하단)
		//	GL_COLOR_BUFFER_BIT, GL_NEAREST
		//);

		//// 디버깅 끝났으면 다시 기본 프레임버퍼 바인딩
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	auto gBuffer = scene->GetGeometryPass()->GetGBuffer();
	auto postFBO = scene->GetPostProcessPass()->GetFramebuffer();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->Get());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postFBO->Get());

	glBlitFramebuffer
	(
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		GL_DEPTH_BUFFER_BIT, // 깊이만 복사!
		GL_NEAREST
	);

	//// [패스 3] 스카이박스 패스: m_frameBuffer에 스카이박스 덧그리기
	scene->GetSkyboxPass()->Render(scene, camera);

	//// [패스 4] 후처리 패스: m_frameBuffer의 결과를 화면에 Resolve
	scene->GetPostProcessPass()->Render(scene, camera);
}

/*===================//
//   render methods  //
//===================*/
// TODO : 만약에 그림자, Postprocessing 등이 없는 경우에 대해서도 따로 처리가 필요하다.
void DevRenderer::RenderMainPass(Scene* scene, Camera* camera, SpotLight* mainLight)
{
	scene->GetPostProcessPass()->BeginDraw();

	ShadowPass* shadowPass = scene->GetShadowPass();
	TexturePtr shadowMap = shadowPass->GetDepthMap();
	glm::mat4 lightSpaceMatrix = shadowPass->GetLightSpaceMatrix();
	glActiveTexture(GL_TEXTURE0 + 10);
	glBindTexture(GL_TEXTURE_2D, shadowMap->Get());

	for (const auto& [name, pass] : scene->GetRenderPasses())
	{
		auto& program = pass->GetProgram();
		program.Use();
		program.SetUniform("shadowMap", 10);
		program.SetUniform("lightSpaceMatrix", lightSpaceMatrix);
	}

	for (const auto& [name, pass] : scene->GetRenderPasses())
	{
		pass->Render(scene, camera);
	}
}
