#include "EnginePch.h"
#include "StandardDeferredLightingPass.h"

#include "Core/Scene.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h" 
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture.h"   
#include "Graphics/ShadowMap.h"
#include "Components/Camera.h"
#include "Components/SpotLight.h"
#include "Components/Transform.h"

StandardDeferredLightingPassUPtr StandardDeferredLightingPass::Create()
{
	auto pass = StandardDeferredLightingPassUPtr(new StandardDeferredLightingPass());
	if (!pass->Init()) return nullptr;
	return std::move(pass);
}

bool StandardDeferredLightingPass::Init()
{
	m_deferredLightProgram = Program::Create
	(
		"./Engine/Shaders/deferred_light.vert",
		"./Engine/Shaders/deferred_light_ubo.frag"
	);
	if (!m_deferredLightProgram) return false;

	m_plane = StaticMesh::CreateNDCQuad();
	if (!m_plane) return false;

	return true;
}

// TODO : Render 추상 메서드 생김새를 조금 다듬을 필요는 있음
void StandardDeferredLightingPass::Render(Scene* scene, Camera* camera)
{
	// 1. G-Buffer 정보  가져오기
	auto geometryPass = scene->GetGeometryPass();
	if (!geometryPass) return;
	auto gBuffer = geometryPass->GetGBuffer();

	// 2. 그리기 준비 (화면 혹은 포스트 프로세싱 FBO에 그림)
	auto postProcessPass = scene->GetPostProcessPass();
	if (postProcessPass)
	{
		postProcessPass->BeginDraw();
		auto ppFBO = postProcessPass->GetFramebuffer();
		glViewport(0, 0, ppFBO->GetWidth(), ppFBO->GetHeight());
	}
	else
	{
		Framebuffer::BindToDefault();
		glViewport(0, 0, gBuffer->GetWidth(), gBuffer->GetHeight());
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	m_deferredLightProgram->Use();

	// Slot 0: Position
	glActiveTexture(GL_TEXTURE0);
	gBuffer->GetColorAttachment(0)->Bind();
	m_deferredLightProgram->SetUniform("gPosition", 0);

	// Slot 1: Normal + shininess
	glActiveTexture(GL_TEXTURE1);
	gBuffer->GetColorAttachment(1)->Bind();
	m_deferredLightProgram->SetUniform("gNormal", 1);

	// Slot 2: Albedo + Specular
	glActiveTexture(GL_TEXTURE2);
	gBuffer->GetColorAttachment(2)->Bind();
	m_deferredLightProgram->SetUniform("gAlbedoSpec", 2);

	// Slot 3: Emission
	glActiveTexture(GL_TEXTURE3);
	gBuffer->GetColorAttachment(3)->Bind();
	m_deferredLightProgram->SetUniform("gEmission", 3);

	// 2. Shadow Map 가져오기 & 바인딩
	auto shadowPass = scene->GetShadowPass();
	if (shadowPass)
	{
		// 1. 텍스처 8장 바인딩
		for (int i = 0; i < 8; ++i)
		{
			glActiveTexture(GL_TEXTURE4 + i);

			// 부모 클래스의 메서드 바로 호출
			auto sm = shadowPass->GetShadowMap(i);
			if (sm) sm->GetShadowMap()->Bind();
			std::string uniformName = "shadowMaps[" + std::to_string(i) + "]";
			m_deferredLightProgram->SetUniform(uniformName, 4 + i);
		}

		// 2. 행렬 전송 (이건 Light 컴포넌트에서 가져오므로 동일)
		const auto& lights = scene->GetLights();
		for (auto* light : lights)
		{
			int32 idx = light->GetShadowMapIndex();
			if (idx >= 0 && idx < 8)
			{
				std::string uName = "lightSpaceMatrices[" + std::to_string(idx) + "]";
				m_deferredLightProgram->SetUniform(uName, light->GetLightSpaceMatrix());
			}
		}
	}

	// 3. 그림 그리기
	m_plane->Draw(m_program.get());
	glEnable(GL_DEPTH_TEST);
}