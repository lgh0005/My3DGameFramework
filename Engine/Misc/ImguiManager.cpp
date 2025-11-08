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
	// ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());

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
	// ImGuizmo::BeginFrame();
}

void ImGuiManager::EndFrame()
{
	if (!m_isEnable) return;
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool ImGuiManager::Begin(const char* name, bool* p_open, ImGuiWindowFlags flags)
{
	if (!m_isEnable) return false;
	return ImGui::Begin(name, p_open, flags);
}

void ImGuiManager::End()
{
	if (!m_isEnable) return;
	ImGui::End();
}

