#include "RuntimePch.h"
#include "GUIConsoleViewport.h"
#include "Managers/ConsoleManager.h"

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
			// 1. Clear 버튼 추가
			if (ImGui::Button("Clear"))
			{
				MGF_CONSOLE.ClearLogs();
			}
			ImGui::Separator();

			// 2. 로그가 많을 경우를 대비해 스크롤 영역 생성
			ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

			// 3. ConsoleManager에서 실제 로그 데이터를 받아와 출력
			const auto& logs = MGF_CONSOLE.GetLogs();
			for (const auto& log : logs)
			{
				ImVec4 color;
				switch (log.level)
				{
				case ELogLevel::Info:    color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); break; // Green
				case ELogLevel::Warning: color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); break; // Yellow
				case ELogLevel::Error:   color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); break; // Red
				}

				// %s를 사용하여 string을 안전하게 출력
				ImGui::TextColored(color, "%s", log.text.c_str());
			}

			ImGui::EndChild();
		}
		ImGui::End();
	}
}