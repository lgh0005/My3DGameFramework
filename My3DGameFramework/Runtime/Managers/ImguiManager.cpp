#include "EnginePch.h"
#include "ImguiManager.h"

void ImGuiManager::Init(bool enable)
{
	if (!enable)
	{
		m_isEnable = enable;
		return;
	}

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(WINDOW.GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 460 core");

	LOG_INFO("ImGUIManager Initialized Successfully.");
}

void ImGuiManager::Clear()
{
	if (!m_isEnable) return;
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	LOG_INFO("ImGUIManager Cleared.");
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

