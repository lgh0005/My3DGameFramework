#include "RuntimePch.h"
#include "GUIConsoleViewport.h"

namespace MGF3D
{
	ConsoleViewport::ConsoleViewport() = default;
	ConsoleViewport::~ConsoleViewport() = default;

	ConsoleViewportUPtr ConsoleViewport::Create(const String& name)
	{
		auto window = ConsoleViewportUPtr(new ConsoleViewport());
		if (!window->Init(name)) return nullptr;
		return window;
	}

	bool ConsoleViewport::Init(const String& name)
	{
		m_name = name;
		m_isOpen = true;
		return true;
	}

	void ConsoleViewport::OnDraw()
	{
		if (ImGui::Begin(m_name.c_str(), &m_isOpen))
		{
			// 레이아웃 확인용 더미 로그 출력
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[Info] MGF3D Game Framework Engine Initialized.");
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[Warning] Sampler object missing in Material 'SharedCubeMaterial'. Using default.");
			ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "[Error] Failed to load audio source: test_bgm.wav");
		}
		ImGui::End();
	}
}