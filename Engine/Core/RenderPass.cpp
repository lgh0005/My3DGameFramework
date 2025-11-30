#include "EnginePch.h"
#include "RenderPass.h"
#include "Core/Scene.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Graphics/Program.h"
#include "Graphics/ShadowMap.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Texture.h"

/*=======================//
//   RenderPass methods  //
//=======================*/
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