#include "../pch.h"
#include "DevRenderer.h"

#include "Core/Scene.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Components/SpotLight.h"
#include "Components/Animator.h"
#include "Core/GameObject.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture.h"
#include "Graphics/Material.h"
#include "Graphics/CubeTexture.h"
#include "Graphics/Image.h"

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
	auto* camera = scene->GetActiveCamera();
	if (!camera) return;

	const auto& lights = scene->GetLights();
	const auto& renderables = scene->GetAllMeshes();

	m_frameBuffer->Bind();

	glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	auto projection = camera->GetProjectionMatrix();
	auto view = camera->GetViewMatrix();
	auto cameraPos = camera->GetTransform().GetPosition();

	// --- 조명 유니폼 설정 : 현재는 하나의 조명만을 취급 ---
	SpotLight* mainLight = nullptr;
	for (auto* light : lights) 
	{
		if (light->GetLightType() == LightType::Spot) 
		{
			mainLight = static_cast<SpotLight*>(light);
			break;
		}
	}

	if (mainLight)
	{
		// 셰이더에 공통적으로 적용할 조명 정보
		auto& lightTransform = mainLight->GetOwner()->GetTransform();
		glm::vec3 lightPos = lightTransform.GetPosition();
		glm::vec3 lightDir = lightTransform.GetForwardVector();
		glm::vec2 cutoff = mainLight->GetCutoff();
		cutoff = glm::vec2(cosf(glm::radians(cutoff[0])), cosf(glm::radians(cutoff[0] + cutoff[1])));
		glm::vec3 attenuation = Utils::GetAttenuationCoeff(mainLight->GetDistance());
		glm::vec3 ambient = mainLight->GetAmbient();
		glm::vec3 diffuse = mainLight->GetDiffuse();
		glm::vec3 specular = mainLight->GetSpecular();

		// m_lighting2 (정적 메시용)
		m_lighting2->Use();
		m_lighting2->SetUniform("viewPos", cameraPos);
		m_lighting2->SetUniform("light.position", lightPos);
		m_lighting2->SetUniform("light.direction", lightDir);
		m_lighting2->SetUniform("light.cutoff", cutoff);
		m_lighting2->SetUniform("light.attenuation", attenuation);
		m_lighting2->SetUniform("light.ambient", ambient);
		m_lighting2->SetUniform("light.diffuse", diffuse);
		m_lighting2->SetUniform("light.specular", specular);

		// TODO : 이후에 조명의 영향을 받도록 셰이더 수정 필요
		// m_skinningProgram (애니메이션 모델용)
		m_skinningLightProgram->Use();
		m_skinningLightProgram->SetUniform("viewPos", cameraPos);
		m_skinningLightProgram->SetUniform("light.position", lightPos);
		m_skinningLightProgram->SetUniform("light.direction", lightDir);
		m_skinningLightProgram->SetUniform("light.cutoff", cutoff);
		m_skinningLightProgram->SetUniform("light.attenuation", attenuation);
		m_skinningLightProgram->SetUniform("light.ambient", ambient);
		m_skinningLightProgram->SetUniform("light.diffuse", diffuse);
		m_skinningLightProgram->SetUniform("light.specular", specular);
	}

	// --- 렌더링 루프 ---
	for (const auto* meshRenderer : renderables)
	{
		GameObject* go = meshRenderer->GetOwner();
		auto mesh = meshRenderer->GetMesh();
		auto& transform = go->GetTransform();
		auto material = meshRenderer->GetMaterial();
		Animator* animator = go->GetComponent<Animator>();
		if (!mesh || !material) continue;

		if (go->GetComponent<Light>())
		{
			// --- 조명 큐브 그리기 ---
			m_simpleProgram->Use();
			auto lightModel = transform.GetModelMatrix();
			auto lightMvp = projection * view * lightModel;
			m_simpleProgram->SetUniform("transform", lightMvp);
			if (mainLight && go == mainLight->GetOwner())
				m_simpleProgram->SetUniform("color", glm::vec4(mainLight->GetAmbient() + mainLight->GetDiffuse(), 1.0f));
			mesh->Draw(m_simpleProgram.get());
		}
		else if (animator)
		{
			// --- Skinning Path (애니메이션 모델) ---
			m_skinningLightProgram->Use();
			auto AnimModel = transform.GetModelMatrix();
			m_skinningLightProgram->SetUniform("projection", projection);
			m_skinningLightProgram->SetUniform("view", view);
			m_skinningLightProgram->SetUniform("model", AnimModel);

			// finalBoneMatrices 유니폼 배열 설정
			auto finalMatrices = animator->GetFinalBoneMatrices();
			for (int i = 0; i < finalMatrices.size(); ++i) 
				m_skinningLightProgram->SetUniform("finalBoneMatrices[" + std::to_string(i) + "]", finalMatrices[i]);

			// 셰이더가 "material.diffuse"를 사용하므로 재질 설정
			material->SetToProgram(m_skinningLightProgram.get());
			mesh->Draw(m_skinningLightProgram.get());
		}
		else
		{
			// --- Static Mesh Path (바닥, 큐브 등) ---
			m_lighting2->Use();
			auto modelTransform = transform.GetModelMatrix();
			auto mvp = projection * view * modelTransform;
			m_lighting2->SetUniform("transform", mvp);
			m_lighting2->SetUniform("modelTransform", modelTransform);

			material->SetToProgram(m_lighting2.get());
			mesh->Draw(m_lighting2.get());
		}
	}

	// --- 추가 : 환경맵 큐브 그리기 [테스트]
	{
		auto transform = Transform::Create();
		transform->SetPosition(glm::vec3(-3.0f, 0.75f, 0.0f));
		m_envMapProgram->Use();
		m_envMapProgram->SetUniform("model", transform->GetModelMatrix());
		m_envMapProgram->SetUniform("view", view);
		m_envMapProgram->SetUniform("projection", projection);
		m_envMapProgram->SetUniform("cameraPos", cameraPos);
		m_cubeTexture->Bind();
		m_envMapProgram->SetUniform("skybox", 0);
		m_box->Draw(m_envMapProgram.get());
	}

	// --- 2단계: [핵심] 스카이박스 하드코딩 렌더링 ---
	// (Scene과 무관하게 Renderer가 직접 실행)
	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_FRONT);

	m_skyboxProgram->Use();

	auto skyboxView = glm::mat4(glm::mat3(view));
	auto transform = projection * skyboxView;
	m_skyboxProgram->SetUniform("transform", transform);

	m_cubeTexture->Bind();
	m_skyboxProgram->SetUniform("skybox", 0);
	m_box->Draw(m_skyboxProgram.get());

	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);

	// --- 3단계: 후처리 (화면) ---
	Framebuffer::BindToDefault();
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

bool DevRenderer::Init(int32 width, int32 height)
{
	// 1. m_simpleProgram 초기화
	{
		m_simpleProgram = Program::Create
		(
			"./Resources/Shaders/simple.vert",
			"./Resources/Shaders/simple.frag"
		);
		if (!m_simpleProgram) return false;
	}

	// 2. m_program 초기화
	{
		m_lighting2 = Program::Create
		(
			"./Resources/Shaders/lighting2.vert", 
			"./Resources/Shaders/lighting2.frag"
		);
		if (!m_lighting2) return false;
	}

	// 2. m_program 초기화
	{
		m_program = Program::Create
		(
			"./Resources/Shaders/texture.vert",
			"./Resources/Shaders/texture.frag"
		);
		if (!m_program) return false;
	}

	// 3. 프레임 버퍼 생성
	{
		m_postProgram = Program::Create(
			"./Resources/Shaders/PostProcessing/postprocess.vert",
			"./Resources/Shaders/PostProcessing/postprocess.frag"
		);
		m_plane = Mesh::CreatePlane();
		if (!m_plane) return false;

		m_frameBuffer = Framebuffer::Create(Texture::Create(width, height, GL_RGBA));
		if (!m_frameBuffer) return false;
	}

	// 4. m_skinningProgram 초기화
	{
		m_skinningProgram = Program::Create
		("./Resources/Shaders/skinning.vert",
			"./Resources/Shaders/skinning.frag"
		);
		if (!m_skinningProgram) return false;
	}

	// 5. m_skinningLightProgram 초기화
	{
		m_skinningLightProgram = Program::Create
		(
			"./Resources/Shaders/skinningLight.vert",
			"./Resources/Shaders/skinningLight.frag"
		);
		if (!m_skinningLightProgram) return false;
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
		m_box = Mesh::CreateBox();
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

	return true;
}

