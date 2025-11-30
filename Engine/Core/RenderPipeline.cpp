#include "EnginePch.h"
#include "RenderPipeline.h"
#include "Core/RenderPass.h"

void RenderPipeline::AddRenderPass(const std::string& name, RenderPassUPtr renderPass)
{
	auto it = m_renderPasses.find(name);
	if (it != m_renderPasses.end())
		SPDLOG_WARN("RenderPass '{}' already exists. Overwriting.", name);
	m_renderPasses[name] = std::move(renderPass);
}

RenderPass* RenderPipeline::GetRenderPass(const std::string& name)
{
	auto it = m_renderPasses.find(name);
	if (it != m_renderPasses.end()) return it->second.get();
	return nullptr;
}