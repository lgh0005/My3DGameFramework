#include "EnginePch.h"
#include "UniversalRenderPipeline.h"

UniversalRenderPipelineUPtr UniversalRenderPipeline::Create()
{
	auto renderPipeline = UniversalRenderPipelineUPtr(new UniversalRenderPipeline());
	if (!renderPipeline->Init()) return nullptr;
	return std::move(renderPipeline);
}

bool UniversalRenderPipeline::Init()
{
	// TODO : PBR 렌더링을 위한 URP 렌더패스들을 Create

	return true;
}

void UniversalRenderPipeline::Render(Scene* scene)
{
	// TODO : PBR 렌더링을 위한 URP 렌더패스들 지나면서 Context 기반 렌더링 수행

	/*=========================//
	//   imgui debug context   //
	//=========================*/
	RenderIMGUIContext();
}

/*======================//
//  debug imgui method  //
//======================*/
#pragma region IMGUI_DEBUG_METHOD

void UniversalRenderPipeline::RenderIMGUIContext()
{
	IMGUI.BeginFrame();

	if (IMGUI.Begin("Test Window"))
	{
		ImGui::Text("Default Render Pipeline #2");
		ImGui::Text("Universal Render Pipeline (PBR)");
	} IMGUI.End();

	IMGUI.EndFrame();
}

#pragma endregion