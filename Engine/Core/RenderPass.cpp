#include "EnginePch.h"
#include "RenderPass.h"
#include "Core/Scene.h"
#include "Core/RenderContext.h"
#include "Components/Camera.h"

std::vector<glm::mat4> RenderPass::s_identityBones;
RenderPass::RenderPass()
{
    if (s_identityBones.empty()) s_identityBones.assign(MAX_BONES, glm::mat4(1.0f));
}
const std::vector<glm::mat4>& RenderPass::GetIdentityBones() const
{
	return s_identityBones;
}

GeneralRenderPass::GeneralRenderPass() = default;
GeneralRenderPass::~GeneralRenderPass() = default;

ContextRenderPass::ContextRenderPass() = default;
ContextRenderPass::~ContextRenderPass() = default;