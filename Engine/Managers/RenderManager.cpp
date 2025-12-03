#include "EnginePch.h"
#include "RenderManager.h"
//#include "Core/Renderer.h"
//#include "Core/RenderPipeline.h"
#include "SRP/StandardRenderPipeline.h"

bool RenderManager::Init()
{
	// 1. 렌더러 생성
	m_renderer = Renderer::Create();
	if (!m_renderer) return false;

	// TODO : 이제 이건 외부에서 수행해야 함.
	// 2. 기본 파이프라인("Standard") 자동 등록 및 기본 렌더링 파이프라인 설정
	RegisterPipeline<StandardRenderPipeline>("Standard");
	SetPipeline("Standard");

	return true;
}

void RenderManager::SetPipeline(const std::string& name)
{
	if (!m_renderer) return;

	auto it = m_pipelines.find(name);
	if (it == m_pipelines.end())
	{
		SPDLOG_ERROR("Failed to find pipeline factory: {}", name);
		return;
	}

	RenderPipelineUPtr newPipeline = it->second();
	m_renderer->SetPipeline(std::move(newPipeline));

	// 리사이즈 동기화
	int32 width, height;
	glfwGetFramebufferSize(WINDOW.GetWindow(), &width, &height);
	m_renderer->OnResize(width, height);

	SPDLOG_INFO("Switched RenderPipeline to: {}", name);
}

void RenderManager::UpdateViewport(int32* outWidth, int32* outHeight)
{
	int32 width, height;
	glfwGetFramebufferSize(WINDOW.GetWindow(), &width, &height);
	OnResize(width, height);

	if (outWidth) *outWidth = width;
	if (outHeight) *outHeight = height;

	SPDLOG_INFO("Viewport synced to window size: {}x{}", width, height);
}

void RenderManager::Clear()
{
	m_renderer.reset();
}

void RenderManager::Render(Scene* scene)
{
	if (m_renderer) m_renderer->Render(scene);
}

void RenderManager::OnResize(int32 width, int32 height)
{
	if (m_renderer) m_renderer->OnResize(width, height);
}