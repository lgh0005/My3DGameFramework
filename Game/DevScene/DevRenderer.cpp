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

void DevRenderer::Render(Scene* scene)
{
	/*====================================//
	//   get essential scene properties   //
	//====================================*/
	// 메인 카메라 속성 가져오기
	auto* camera = scene->GetMainCamera();
	if (!camera) return;

	// 메인 조명 속성 가져오기
	SpotLight* mainLight = static_cast<SpotLight*>(scene->GetMainLight());

	/*================================//
	//   main scene rendergin logic   //
	//================================*/
	// [패스 1] 그림자 패스: m_shadowMap에 깊이 정보 기록
	scene->GetShadowPass()->Render(scene, camera);

	// [패스 2] 디퍼드 셰이딩 및 포스트 프로세스 프레임 버퍼에 깊이 복사
	scene->GetGeometryPass()->Render(scene, camera);
	scene->GetDeferredLightingPass()->Render(scene, camera);

	auto gBuffer = scene->GetGeometryPass()->GetGBuffer();
	auto postFBO = scene->GetPostProcessPass()->GetFramebuffer();
	BlitCopyDepth(gBuffer, postFBO, gBuffer->GetWidth(), gBuffer->GetHeight());

	// [패스 3] 포워드 셰이딩 
	for (const auto& [name, pass] : scene->GetRenderPasses())
		pass->Render(scene, camera);

	// [패스 4] 스카이박스 패스: m_frameBuffer에 스카이박스 덧그리기
	scene->GetSkyboxPass()->Render(scene, camera);

	// [패스 5] 후처리 패스: m_frameBuffer의 결과를 화면에 Resolve
	scene->GetPostProcessPass()->Render(scene, camera);
}