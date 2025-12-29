#include "EnginePch.h"
#include "RenderManager.h"

bool RenderManager::Init()
{
	m_renderer = Renderer::Create();
	if (!m_renderer) return false;
	return true;
}

void RenderManager::SetPipeline(const std::string& name)
{
	if (!m_renderer) return;

	auto it = m_pipelines.find(name);
	if (it == m_pipelines.end())
	{
		LOG_ERROR("Failed to find pipeline factory: {}", name);
		return;
	}

	RenderPipelineUPtr newPipeline = it->second();
	m_renderer->SetPipeline(std::move(newPipeline));

	// 리사이즈 동기화
	int32 width, height;
	glfwGetFramebufferSize(WINDOW.GetWindow(), &width, &height);
	m_renderer->OnResize(width, height);

	LOG_INFO("Switched RenderPipeline to: {}", name);
}

void RenderManager::UpdateViewport(int32* outWidth, int32* outHeight)
{
	int32 width, height;
	glfwGetFramebufferSize(WINDOW.GetWindow(), &width, &height);
	OnResize(width, height);

	if (outWidth) *outWidth = width;
	if (outHeight) *outHeight = height;

	LOG_INFO("Viewport synced to window size: {}x{}", width, height);
}

void RenderManager::Clear()
{
	m_renderer.reset();
	LOG_INFO("RenderManager Cleared.");
}

void RenderManager::Render(Scene* scene)
{
	if (m_renderer) m_renderer->Render(scene);
}

void RenderManager::OnResize(int32 width, int32 height)
{
	if (m_renderer) m_renderer->OnResize(width, height);
}