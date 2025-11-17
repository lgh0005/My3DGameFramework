#include "EnginePch.h"
#include "RenderPass.h"
#include "Graphics/Program.h"
#include "Core/Scene.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"

RenderPass::~RenderPass() = default;

bool RenderPass::Init(ProgramUPtr program)
{
	m_program = std::move(program);
	if (!m_program) return false;
	return true;
}

void RenderPass::AddRenderer(MeshRenderer* renderer)
{
	m_renderers.push_back(renderer);
}

const std::vector<MeshRenderer*>& RenderPass::GetRenderers() const
{
	return m_renderers;
}
