#include "EnginePch.h"
#include "StandardDeferredLightingPass.h"

#include "Core/Scene.h"
#include "SRP/StandardRenderPipeline.h"
#include "SRP/RenderPasses/StandardGeometryPass.h"
#include "SRP/RenderPasses/StandardPostProcessPass.h"
#include "SRP/RenderPasses/StandardShadowPass.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h" 
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture.h"   
#include "Graphics/ShadowMap.h"
#include "Components/Camera.h"
#include "Components/SpotLight.h"
#include "Components/Transform.h"

#include "SRP/StandardRenderContext.h"

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
		"./Resources/Shaders/Standard/Deferred_LightPass.vert",
		"./Resources/Shaders/Standard/Deferred_LightPass.frag"
	);
	if (!m_deferredLightProgram) return false;

	m_plane = StaticMesh::CreateNDCQuad();
	if (!m_plane) return false;

	return true;
}

// TODO : Render 추상 메서드 생김새를 조금 다듬을 필요는 있음
void StandardDeferredLightingPass::Render(Scene* scene, Camera* camera)
{
	auto pipeline = (StandardRenderPipeline*)(RENDER.GetRenderer()->GetPipeline());

	// 1. G-Buffer 정보  가져오기
	auto geometryPass = pipeline->GetGeometryPass();
	if (!geometryPass) return;
	auto gBuffer = geometryPass->GetGBuffer();

	// 2. 그리기 준비 (화면 혹은 포스트 프로세싱 FBO에 그림)
	auto postProcessPass = pipeline->GetPostProcessPass();
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

	// Slot 12 : SSAO Texture Binding
	if (m_ssaoTexture)
	{
		glActiveTexture(GL_TEXTURE12);
		m_ssaoTexture->Bind();
		m_deferredLightProgram->SetUniform("ssaoTexture", 12);
		m_deferredLightProgram->SetUniform("useSSAO", true);
	}
	else
	{
		m_deferredLightProgram->SetUniform("useSSAO", false);
	}

	// 2. Shadow Map 가져오기 & 바인딩
	auto shadowPass = pipeline->GetShadowPass();
	if (shadowPass)
	{
		// 1. 텍스처 8장 바인딩
		for (int i = 0; i < MAX_SHADOW_CASTER; ++i)
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
	m_plane->Draw(m_deferredLightProgram.get());
	glEnable(GL_DEPTH_TEST);
	m_ssaoTexture = nullptr;
}

void StandardDeferredLightingPass::TestRender(RenderContext* context)
{
	// 0. Context 캐스팅 및 유효성 검사
	auto stdCtx = (StandardRenderContext*)context;
	if (!stdCtx) return;

	// 1. Context에서 G-Buffer 텍스처 가져오기
	Texture* tPos = stdCtx->GetGBufferPosition();
	Texture* tNormal = stdCtx->GetGBufferNormal();
	Texture* tAlbedo = stdCtx->GetGBufferAlbedo();
	Texture* tEmission = stdCtx->GetGBufferEmission();
	if (!tPos || !tNormal || !tAlbedo || !tEmission) return;

	// 2. 그리기 준비 (Output FBO 설정)
	Framebuffer* targetFBO = stdCtx->GetTargetFramebuffer();
	if (targetFBO)
	{
		targetFBO->Bind();
		glViewport(0, 0, targetFBO->GetWidth(), targetFBO->GetHeight());
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	else
	{
		Framebuffer::BindToDefault();
		// 텍스처 크기를 기준으로 뷰포트 설정
		glViewport(0, 0, tPos->GetWidth(), tPos->GetHeight());
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	m_deferredLightProgram->Use();

	// 3. G-Buffer 텍스처 바인딩 (Context 데이터 사용)
	glActiveTexture(GL_TEXTURE0); tPos->Bind();
	m_deferredLightProgram->SetUniform("gPosition", 0);

	glActiveTexture(GL_TEXTURE1); tNormal->Bind();
	m_deferredLightProgram->SetUniform("gNormal", 1);

	glActiveTexture(GL_TEXTURE2); tAlbedo->Bind();
	m_deferredLightProgram->SetUniform("gAlbedoSpec", 2);

	glActiveTexture(GL_TEXTURE3); tEmission->Bind();
	m_deferredLightProgram->SetUniform("gEmission", 3);

	// 4. SSAO 텍스처 바인딩 (Context 데이터 사용)
	Texture* tSSAO = stdCtx->GetSSAOTexture();
	if (tSSAO)
	{
		glActiveTexture(GL_TEXTURE12);
		tSSAO->Bind();
		m_deferredLightProgram->SetUniform("ssaoTexture", 12);
		m_deferredLightProgram->SetUniform("useSSAO", true);
	}
	else
	{
		m_deferredLightProgram->SetUniform("useSSAO", false);
	}

	// 5. Shadow Maps 바인딩 (Pipeline 데이터 사용 - 기존 유지)
	for (int i = 0; i < MAX_SHADOW_CASTER; ++i)
	{
		glActiveTexture(GL_TEXTURE4 + i);

		Texture* shadowMap = stdCtx->GetShadowMap(i);
		if (shadowMap) shadowMap->Bind();

		std::string uniformName = "shadowMaps[" + std::to_string(i) + "]";
		m_deferredLightProgram->SetUniform(uniformName, 4 + i);
	}

	// 6. Light Matrices 전송(Context의 Culled List 사용)
	// Scene 전체 조명이 아니라, Context에 담긴 조명만 처리
	const auto& lights = stdCtx->GetLights();
	for (auto* light : lights)
	{
		int32 idx = light->GetShadowMapIndex();
		if (idx >= 0 && idx < MAX_SHADOW_CASTER)
		{
			std::string uName = "lightSpaceMatrices[" + std::to_string(idx) + "]";
			m_deferredLightProgram->SetUniform(uName, light->GetLightSpaceMatrix());
		}
	}

	// 7. 그리기
	m_plane->Draw(m_deferredLightProgram.get());
	glEnable(GL_DEPTH_TEST);
}