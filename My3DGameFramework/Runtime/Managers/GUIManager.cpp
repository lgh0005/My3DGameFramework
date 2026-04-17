#include "RuntimePch.h"
#include "GUIManager.h"
#include "Managers/WindowManager.h"

namespace MGF3D
{
	GUIManager::GUIManager() = default;
	GUIManager::~GUIManager() = default;

	void GUIManager::Init(bool enable)
	{
		if (!enable)
		{
			m_isEnable = enable;
			return;
		}

		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());

		ImGui_ImplGlfw_InitForOpenGL(MGF_WINDOW.GetNativeHandle(), true);
		ImGui_ImplOpenGL3_Init("#version 460 core");

		MGF_LOG_INFO("GUIManager Initialized Successfully.");
	}

	void GUIManager::Clear()
	{
		if (!m_isEnable) return;
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		MGF_LOG_INFO("GUIManager Cleared.");
	}

	void GUIManager::BeginFrame()
	{
		if (!m_isEnable) return;
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void GUIManager::EndFrame()
	{
		if (!m_isEnable) return;
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	bool GUIManager::Begin(cstr name, bool* p_open, ImGuiWindowFlags flags)
	{
		if (!m_isEnable) return false;
		return ImGui::Begin(name, p_open, flags);
	}

	void GUIManager::End()
	{
		if (!m_isEnable) return;
		ImGui::End();
	}
}
