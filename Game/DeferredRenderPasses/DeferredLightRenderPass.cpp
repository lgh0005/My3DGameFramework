#include "pch.h"
#include "DeferredLightRenderPass.h"

#include "Core/Scene.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h" 
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture.h"    
#include "Components/Camera.h"
#include "Components/SpotLight.h"
#include "Components/Transform.h"

DeferredLightPass::~DeferredLightPass() = default;

DeferredLightPassUPtr DeferredLightPass::Create
(
	ProgramUPtr program, 
	MeshPtr screenMesh
)
{
	auto pass = DeferredLightPassUPtr(new DeferredLightPass());
	if (!pass->Init(std::move(program), screenMesh)) return nullptr;
	return std::move(pass);
}

bool DeferredLightPass::Init(ProgramUPtr program, MeshPtr screenMesh)
{
	if (!Super::Init(std::move(program))) return false;
	m_plane = screenMesh;
	return true;
}

void DeferredLightPass::Render(Scene* scene, Camera* camera)
{
	// 1. 그리기 준비 (화면 혹은 포스트 프로세싱 FBO에 그림)
	// TODO : 포스트 프로세서의 프레임 버퍼의 텍스쳐에 그림을 그리도록 만들 필요 있음
	auto postProcessPass = scene->GetPostProcessPass();
	if (postProcessPass)
	{
		// PostProcessing이 있으면 그쪽 FBO에 그립니다.
		postProcessPass->BeginDraw();
	}
	else
	{
		// 없으면 화면에 그립니다.
		// Depth Test는 끄거나, G-Buffer의 Depth를 복사해와야 하는데
		// Lighting Pass 자체는 화면 덮어쓰기므로 일단 끕니다.
		Framebuffer::BindToDefault();
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	m_program->Use();
	auto geometryPass = scene->GetGeometryPass();
	if (!geometryPass) return;
	auto gBuffer = geometryPass->GetGBuffer();

	// Slot 0: Position
	glActiveTexture(GL_TEXTURE0);
	gBuffer->GetColorAttachment(0)->Bind();
	m_program->SetUniform("gPosition", 0);

	// Slot 1: Normal + shininess
	glActiveTexture(GL_TEXTURE1);
	gBuffer->GetColorAttachment(1)->Bind();
	m_program->SetUniform("gNormal", 1);

	// Slot 2: Albedo + Specular
	glActiveTexture(GL_TEXTURE2);
	gBuffer->GetColorAttachment(2)->Bind();
	m_program->SetUniform("gAlbedoSpec", 2);

	// Slot 3: Emission
	glActiveTexture(GL_TEXTURE3);
	gBuffer->GetColorAttachment(3)->Bind();
	m_program->SetUniform("gEmission", 3);

	// 2. Shadow Map 가져오기 & 바인딩
	auto shadowPass = scene->GetShadowPass();
	if (shadowPass)
	{
		// Slot 4: Shadow Map
		glActiveTexture(GL_TEXTURE4);
		shadowPass->GetDepthMap()->Bind();
		m_program->SetUniform("shadowMap", 4);
		m_program->SetUniform("lightSpaceMatrix", shadowPass->GetLightSpaceMatrix());
	}

	// 4. 조명(Light) 및 카메라 정보 전송
	// 메인 라이트 하나만 처리한다고 가정 (여러 개면 반복문이나 UBO 사용)
	// TODO : 본격적으로 Scene에 있는 조명 컴포넌트 벡터들을 통해서 조명 연산들을
	// 누적시킬 필요가 있음.
	Light* mainLight = scene->GetMainLight();
	if (mainLight)
	{
		auto lightTransform = mainLight->GetTransform();
		m_program->SetUniform("light.position", lightTransform.GetPosition());
		m_program->SetUniform("light.direction", lightTransform.GetForwardVector()); // Spot/Dir Light용
		m_program->SetUniform("light.ambient", mainLight->GetAmbient());
		m_program->SetUniform("light.diffuse", mainLight->GetDiffuse());
		m_program->SetUniform("light.specular", mainLight->GetSpecular());

		// Attenuation, Cutoff 등 추가 속성 전송
		// (SpotLight로 캐스팅해서 가져와야 할 수 있음)
		if (mainLight->GetLightType() == LightType::Spot) // 예시
		{
			// TODO : 일부 조명 컴포넌트에 GetAttenuation 메서드를 래핑할 필요 있음
			auto spot = static_cast<SpotLight*>(mainLight);

			glm::vec2 cutoff = spot->GetCutoff();
			cutoff = glm::vec2(cosf(glm::radians(cutoff[0])), cosf(glm::radians(cutoff[0] + cutoff[1])));
			m_program->SetUniform("light.cutoff", cutoff);

			// m_program->SetUniform("light.cutoff", spot->GetCutoff());
			m_program->SetUniform("light.attenuation", Utils::GetAttenuationCoeff(spot->GetDistance()));
		}
	}

	m_program->SetUniform("viewPos", camera->GetTransform().GetPosition());
	m_plane->Draw(m_program.get());
	glEnable(GL_DEPTH_TEST);
}