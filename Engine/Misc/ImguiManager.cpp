#include "EnginePch.h"
#include "ImguiManager.h"

void ImGuiManager::Init(GLFWwindow* handle, bool enable)
{
	if (!enable)
	{
		m_isEnable = enable;
		return;
	}

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(handle, true);
	ImGui_ImplOpenGL3_Init("#version 460 core");
}

void ImGuiManager::ShutDown()
{
	if (!m_isEnable) return;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::BeginFrame()
{
	if (!m_isEnable) return;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::EndFrame()
{
	if (!m_isEnable) return;

	// 렌더링 준비
	ImGui::Render();

	// ImGui가 그린 데이터를 실제 화면에 렌더링
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

