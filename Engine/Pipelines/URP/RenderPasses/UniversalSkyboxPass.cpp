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
	auto skyboxTex = urpCtx->GetSkyboxTexture();
	if (!skyboxTex) return;

	auto projection = camera->GetProjectionMatrix();
	auto view = camera->GetViewMatrix();

	glDepthFunc(GL_LEQUAL);  // 깊이가 1.0(초기값)인 곳에도 그려지도록
	glCullFace(GL_FRONT);

	m_hdrSkyboxProgram->Use();
	m_hdrSkyboxProgram->SetUniform("view", view);
	m_hdrSkyboxProgram->SetUniform("projection", projection);

	// 3. 텍스처 바인딩
	m_hdrSkyboxProgram->SetUniform("skybox", 0);
	glActiveTexture(GL_TEXTURE0);
	skyboxTex->Bind();

	m_cubeMesh->Draw(m_hdrSkyboxProgram.get());

	// 상태 복구
	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);
}

