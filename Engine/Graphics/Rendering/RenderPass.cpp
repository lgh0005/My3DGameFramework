#include "EnginePch.h"
#include "RenderPass.h"
#include "Scene/Scene.h"
#include "Graphics/Rendering/RenderContext.h"
#include "Components/Camera.h"

/*====================================================//
//  Base class for all render passes (Common Data)    //
//====================================================*/
std::vector<glm::mat4> RenderPass::s_identityBones;
RenderPass::RenderPass()
{
    if (s_identityBones.empty()) s_identityBones.assign(MAX_BONES, glm::mat4(1.0f));
}
RenderPass::~RenderPass() = default;
const std::vector<glm::mat4>& RenderPass::GetIdentityBones() const
{
	return s_identityBones;
}

/*===========================================//
//   custom extension render pass interface  //
//===========================================*/
GeneralRenderPass::GeneralRenderPass() = default;
GeneralRenderPass::~GeneralRenderPass() = default;

/*====================================================//
//   RenderContext based core render pass interface   //
//====================================================*/
ContextRenderPass::ContextRenderPass() = default;
ContextRenderPass::~ContextRenderPass() = default;