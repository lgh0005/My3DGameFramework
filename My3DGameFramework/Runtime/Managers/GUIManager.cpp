#include "RuntimePch.h"
#include "GUIManager.h"
#include "Editor/GUIWindow.h"

namespace MGF3D
{
	GUIManager::GUIManager() = default;
	GUIManager::~GUIManager() = default;

	void GUIManager::Init(GLFWwindow* window)
	{
		// 1. ImGui 컨텍스트 생성
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// 2. 핵심 기능 활성화 (도킹 & 멀티 뷰포트)
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 키보드 컨트롤
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // 도킹 기능 활성화
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // 창을 메인 윈도우 밖으로 이동 가능하게 활성화

		// 3. 스타일 설정 (다크 테마)
		ImGui::StyleColorsDark();

		// 뷰포트 사용 시 메인 윈도우 스타일과 이질감이 없도록 보정
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// 4. 플랫폼 및 렌더러 백엔드 초기화
		// 버전 문자열은 사용자님의 OpenGL 설정(#version 460 core)에 맞춥니다.
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460 core");
	}

	void GUIManager::Render()
	{
		// 0. imgui 렌더링 시작
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 1. imgui 윈도우 렌더링
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		
		// 1-1. DockSpace 공간
		ImGui::Begin("MGF3D DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar(3);
		ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		ImGui::End();

		// 1-2. GUI 윈도우들 렌더링
		for (auto& gui : m_guis)
		{
			if (gui->IsOpen()) 
				gui->OnDraw();
		}

		// 2. imgui 렌더링 제출
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// 3. 멀티 뷰포트 창 처리
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void GUIManager::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void GUIManager::AddGUIWindow(GUIWindowUPtr gui)
	{
		m_guis.push_back(std::move(gui));
	}
}
