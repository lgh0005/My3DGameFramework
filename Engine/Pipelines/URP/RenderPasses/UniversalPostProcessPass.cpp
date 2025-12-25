#include "EnginePch.h"
#include "UniversalPostProcessPass.h"

#include "Core/Scene.h"
#include "Core/RenderContext.h"
#include "Graphics/Program.h"
#include "Resources/ScreenMesh.h"
#include "Resources/StaticMesh.h"
#include "Resources/Texture.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Geometry.h"
#include "Components/Camera.h"

#include "Pipelines/URP/UniversalRenderContext.h"

UniversalPostProcessPassUPtr UniversalPostProcessPass::Create(int32 width, int32 height)
{
	auto pass = UniversalPostProcessPassUPtr(new UniversalPostProcessPass());
	if (!pass->Init(width, height)) return nullptr;
	return std::move(pass);
}

bool UniversalPostProcessPass::Init(int32 width, int32 height)
{
	// TODO:
	// 0. 임시 방편으로 라인하르트 톤 매핑 셰이더 프로그램을 생성

	// 이후에는,
	// 1. Kawase bloom 셰이더 프로그램
	// 2. URP Post-processing 셰이더 프로그램

	// 1. 셰이더 로드
	m_toneMappingProgram = Program::Create
	(
		"./Resources/Shaders/Universal/Universal_Post_PostProcess.vert",
		"./Resources/Shaders/Universal/Universal_Post_PostProcess.frag"
	);
	if (!m_toneMappingProgram) return false;

	// 2. 스크린 메쉬 생성
	m_plane = ScreenMesh::Create();
	if (!m_plane) return false;

	// 3. 프레임 버퍼 생성
	m_frameBuffer = Framebuffer::Create(width, height, 1);
	if (!m_frameBuffer) return false;

	return true;
}

void UniversalPostProcessPass::Render(RenderContext* context)
{
	// TODO : 
	// 0. 임시 방편으로 라인하르트 톤 매핑을 해주는 로직을 여기로 옮기기
	
	// 이후에는,
	// 1. Kawase bloom으로 bloom 텍스쳐 뽑아오기
	// 2. FXAA 안티 앨리어싱
	// 3. 톤 매핑

	// 1. 최종 화면(Backbuffer)에 그릴 준비
	// INFO : 이후에 LightPass에 맞춰서 m_frameBuffer->Resolve();
	// 를 호출해야 할 수도 있다.
	Framebuffer::BindToDefault();

	auto hdrTexture = m_frameBuffer->GetColorAttachment(0);

	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_toneMappingProgram->Use();
	m_toneMappingProgram->SetUniform("gamma", m_gamma);
	m_toneMappingProgram->SetUniform("exposure", m_exposure);

	if (hdrTexture)
	{
		glActiveTexture(GL_TEXTURE0);
		hdrTexture->Bind();
		m_toneMappingProgram->SetUniform("screenTexture", 0);
	}

	m_plane->Draw();

	// 상태 복구 (선택 사항)
	glEnable(GL_DEPTH_TEST);
}

void UniversalPostProcessPass::Resize(int32 width, int32 height)
{
	m_frameBuffer = Framebuffer::Create(width, height, 1);
}
