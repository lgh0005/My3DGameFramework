#include "EnginePch.h"
#include "UniversalSkyboxPass.h"

#include "Core/Scene.h"
#include "Graphics/Program.h"
#include "Graphics/Mesh.h"
#include "Graphics/StaticMesh.h"
#include "Graphics/CubeTexture.h"
#include "Graphics/Texture.h"
#include "Graphics/CubeFramebuffer.h"
#include "Graphics/Material.h"
#include "Graphics/SkyLight.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Transform.h"
#include "Core/GameObject.h"

#include "Pipelines/URP/UniversalRenderContext.h"

UniversalSkyboxPassUPtr UniversalSkyboxPass::Create()
{
	auto pass = UniversalSkyboxPassUPtr(new UniversalSkyboxPass());
	if (!pass->Init()) return false;
	return std::move(pass);
}

bool UniversalSkyboxPass::Init()
{
	m_hdrSkyboxProgram = Program::Create
	(
		"./Resources/Shaders/Universal/skybox_hdr.vert",
		"./Resources/Shaders/Universal/skybox_hdr.frag"
	);
	if (!m_hdrSkyboxProgram) return false;

	m_cubeMesh = GeometryGenerator::CreateBox();
	if (!m_cubeMesh) return false;

	return true;
}

void UniversalSkyboxPass::Render(RenderContext* context)
{
	// URP 전용 렌더 컨텍스트로 캐스팅
	auto urpCtx = (UniversalRenderContext*)context;
	auto camera = urpCtx->GetCamera();
	auto skyLight = urpCtx->GetSkyLight()->GetSkybox();
	if (!skyLight) return;

	auto projection = camera->GetProjectionMatrix();
	auto view = camera->GetViewMatrix();

	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_FRONT);

	m_hdrSkyboxProgram->Use();
	auto skyboxView = glm::mat4(glm::mat3(view));
	auto transform = projection * skyboxView;
	m_hdrSkyboxProgram->SetUniform("transform", transform);

	// 텍스처 바인딩
	glActiveTexture(GL_TEXTURE0);
	skyLight->Bind();
	m_hdrSkyboxProgram->SetUniform("skybox", 0);

	m_cubeMesh->Draw(m_hdrSkyboxProgram.get());

	// 상태 복구
	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);
}

