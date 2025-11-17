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
	// 3. 프레임 버퍼 생성
	{
		m_postProgram = Program::Create(
			"./Resources/Shaders/PostProcessing/postprocess.vert",
			"./Resources/Shaders/PostProcessing/postprocess.frag"
		);
		m_plane = StaticMesh::CreatePlane();
		if (!m_plane) return false;

		m_frameBuffer = Framebuffer::Create(width, height, 4);
		if (!m_frameBuffer) return false;
	}

	// 6. m_skybox 초기화
	{
		auto cubeRight = Image::Load("./Resources/Images/Skybox/right.jpg", false);
		auto cubeLeft = Image::Load("./Resources/Images/Skybox/left.jpg", false);
		auto cubeTop = Image::Load("./Resources/Images/Skybox/top.jpg", false);
		auto cubeBottom = Image::Load("./Resources/Images/Skybox/bottom.jpg", false);
		auto cubeFront = Image::Load("./Resources/Images/Skybox/front.jpg", false);
		auto cubeBack = Image::Load("./Resources/Images/Skybox/back.jpg", false);
		m_cubeTexture = CubeTexture::CreateFromImages({
		  cubeRight.get(),
		  cubeLeft.get(),
		  cubeTop.get(),
		  cubeBottom.get(),
		  cubeFront.get(),
		  cubeBack.get(),
			});
		m_box = StaticMesh::CreateBox();
		if (!m_box) return false;
		m_skyboxProgram = Program::Create
		(
			"./Resources/Shaders/Skybox/skybox.vert", 
			"./Resources/Shaders/Skybox/skybox.frag"
		);
	}

	// 7. m_envMapProgram 초기화
	{
		m_envMapProgram = Program::Create
		(
			"./Resources/Shaders/Environment/environment.vert",
			"./Resources/Shaders/Environment/environment.frag"
		);
		if (!m_envMapProgram) return false;
	}

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
	m_frameBuffer->Bind();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	for (const auto& [name, pass] : scene->GetRenderPasses())
	{
		pass->Render(scene, camera);
	}
}

void DevRenderer::RenderSkyboxPass(Scene* scene, Camera* camera)
{
	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_FRONT);

	m_skyboxProgram->Use();

	auto projection = camera->GetProjectionMatrix();
	auto view = camera->GetViewMatrix();
	auto skyboxView = glm::mat4(glm::mat3(view));
	auto transform = projection * skyboxView;
	m_skyboxProgram->SetUniform("transform", transform);

	m_cubeTexture->Bind();
	m_skyboxProgram->SetUniform("skybox", 0);
	m_box->Draw(m_skyboxProgram.get());

	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);
}

void DevRenderer::RenderPostProcessingPass(Scene* scene, Camera* camera)
{
	m_frameBuffer->Resolve();
	Framebuffer::BindToDefault();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	m_postProgram->Use();
	m_postProgram->SetUniform("transform", glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f)));
	m_postProgram->SetUniform("gamma", m_gamma);
	m_frameBuffer->GetColorAttachment()->Bind();
	m_postProgram->SetUniform("tex", 0);
	m_plane->Draw(m_postProgram.get());
}
