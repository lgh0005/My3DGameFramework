#include "EnginePch.h"
#include "SkyboxPass.h"

#include "Scene/Scene.h"
#include "Graphics/RenderContext.h"
#include "Resources/CubeTexture.h"
#include "Resources/Mesh.h"
#include "Resources/StaticMesh.h"
#include "Resources/Program.h"
#include "Resources/Texture.h"
#include "Graphics/Geometry.h"
#include "Graphics/SkyLight.h"
#include "Components/Camera.h"
#include "Framebuffers/PostProcessFramebuffer.h"

#include "Pipelines/SRP/StandardRenderContext.h"

DECLARE_DEFAULTS_IMPL(SkyboxPass)

SkyboxPassUPtr SkyboxPass::Create()
{
	auto pass = SkyboxPassUPtr(new SkyboxPass());
	if (!pass->Init()) return nullptr;
	return std::move(pass);
}

bool SkyboxPass::Init()
{
	m_skyboxProgram = RESOURCE.GetResource<Program>("common_skybox");
	m_cubeMesh = RESOURCE.GetResource<StaticMesh>("Cube");
	return true;
};

void SkyboxPass::Render(RenderContext* context)
{
	// 0. 자신의 렌더 패스에 활용되고 있는 RenderContext로 캐스팅
	auto stdCtx = (StandardRenderContext*)context;
	auto camera = stdCtx->GetCamera();
	auto skyLight = stdCtx->GetSkyLight();
	if (!camera || !skyLight) return;

	auto skyTexture = skyLight->GetSkybox();
	if (!skyTexture) return;

	// 1. 대상 프레임버퍼 바인딩 (PostProcessFramebuffer 등)
	auto* target = stdCtx->GetTargetFramebuffer();
	if (target)
	{
		target->Bind();
		glViewport(0, 0, target->GetWidth(), target->GetHeight());
	}
	else
	{
		Framebuffer::BindToDefault();
		auto* gPos = stdCtx->GetGBufferPosition();
		if (gPos) glViewport(0, 0, gPos->GetWidth(), gPos->GetHeight());
	}

	// 2. 렌더링 상태 설정
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_FRONT);

	// 3. 셰이더 설정
	m_skyboxProgram->Use();

	auto projection = camera->GetProjectionMatrix();
	auto view = camera->GetViewMatrix();

	// 회전만 남기기 (Translation 제거)
	auto skyboxView = glm::mat4(glm::mat3(view));
	auto transform = projection * skyboxView;

	m_skyboxProgram->SetUniform("transform", transform);

	// 4. 텍스처 바인딩 및 그리기
	glActiveTexture(GL_TEXTURE0);
	skyTexture->Bind();
	m_skyboxProgram->SetUniform("skybox", 0);

	m_cubeMesh->Draw();

	// 5. 상태 복구
	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);
}