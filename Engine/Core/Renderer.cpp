#include "EnginePch.h"
#include "Renderer.h"
#include "Core/Scene.h"
#include "Graphics/FrameBuffer.h"
#include "Core/RenderPipeline.h"
#include "Pipelines/SRP/StandardRenderPipeline.h"

RendererUPtr Renderer::Create()
{
	auto renderer = RendererUPtr(new Renderer());
	renderer->Init();
	return std::move(renderer);
}

void Renderer::Init()
{
	// 기본 OpenGL 상태 설정 (초기화)
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void Renderer::Render(Scene* scene)
{
	// 파이프라인이 아직 세팅되지 않았으면 무시 (안전장치)
	if (m_pipeline)
	{
		m_pipeline->Render(scene);
	}
	else
	{
		// 파이프라인이 없을 때 화면 클리어
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

// TODO : 리사이징 로직 검토 필요
void Renderer::OnResize(int32 width, int32 height)
{
	m_pipeline->OnResize(width, height);
}

void Renderer::SetPipeline(RenderPipelineUPtr pipeline)
{
	m_pipeline = std::move(pipeline);
}