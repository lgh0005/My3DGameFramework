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
	if (!devRenderer->Init(width, height)) return nullptr;
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
	// const auto& renderables = scene->GetAllMeshes();

	// [패스 1] 그림자 패스: m_shadowMap에 깊이 정보 기록
	RenderShadowPass(scene, camera, mainLight);

	// [패스 2] 메인 씬 패스: m_frameBuffer에 씬 렌더링
	RenderMainPass(scene, camera, mainLight);

	// [패스 3] 스카이박스 패스: m_frameBuffer에 스카이박스 덧그리기
	RenderSkyboxPass(scene, camera);

	// [패스 4] 후처리 패스: m_frameBuffer의 결과를 화면에 Resolve
	RenderPostProcessingPass(scene, camera);
}

bool DevRenderer::Init(int32 width, int32 height)
{
	// 9. 셰도우 맾 초기화
	{
		m_shadowMap = ShadowMap::Create(1024, 1024);
		m_shadowDepthProgram = Program::Create
		(
			"./Resources/Shaders/Shadow/Shadow.vert",
			"./Resources/Shaders/Shadow/Shadow.frag"
		);
		if (!m_shadowDepthProgram) return false;
	}

	return true;
}

/*===================//
//   render methods  //
//===================*/
void DevRenderer::RenderShadowPass(Scene* scene, Camera* camera, SpotLight* mainLight)
{
	if (!mainLight) return;
	RenderPass* staticPass = scene->GetRenderPass("Static");
	if (!staticPass) return;
	const int32 SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	auto& lightTransform = mainLight->GetTransform();
	glm::vec3 lightPos = lightTransform.GetPosition();
	glm::vec3 lightDir = lightTransform.GetForwardVector();
	float fov = mainLight->GetCutoff()[0] * 2.0f;
	float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
	glm::mat4 lightProjection = glm::perspective(glm::radians(fov), aspect, 0.1f, mainLight->GetDistance());
	glm::mat4 lightView = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	m_shadowMap->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);

	// 1-3. 섀도우 맵 셰이더 설정 -> 지금 코드가 좀 더러울 수 있음
	m_shadowDepthProgram->Use();
	m_shadowDepthProgram->SetUniform("lightSpaceMatrix", lightSpaceMatrix);

	for (const auto* meshRenderer : staticPass->GetRenderers())
	{
		// (Scene::AddRenderPass("Static", ...)에서 이미 필터링됨)
		auto model = meshRenderer->GetTransform().GetModelMatrix();
		m_shadowDepthProgram->SetUniform("model", model);
		meshRenderer->GetMesh()->Draw(m_shadowDepthProgram.get());
	}

	glCullFace(GL_BACK);
}

void DevRenderer::RenderMainPass(Scene* scene, Camera* camera, SpotLight* mainLight)
{
	auto postProcess = scene->GetRenderPass("PostProcess");
	reinterpret_cast<PostProcessingRenderPass*>(postProcess)->BeginDraw();

	for (const auto& [name, pass] : scene->GetRenderPasses())
	{
		pass->Render(scene, camera);
	}
}

void DevRenderer::RenderSkyboxPass(Scene* scene, Camera* camera)
{
	scene->GetRenderPass("Skybox")->Render(scene, camera);
}

void DevRenderer::RenderPostProcessingPass(Scene* scene, Camera* camera)
{
	auto postProcess = scene->GetRenderPass("PostProcess");
	reinterpret_cast<PostProcessingRenderPass*>(postProcess)->Render(scene, camera);
}
