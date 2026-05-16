#include "RuntimePch.h"
#include "GUIInspectorViewport.h"
#include <imgui.h>

namespace MGF3D
{
	InspectorViewport::InspectorViewport() = default;
	InspectorViewport::~InspectorViewport() = default;

	InspectorViewportUPtr InspectorViewport::Create(const String& name)
	{
		auto window = InspectorViewportUPtr(new InspectorViewport());
		if (!window->Init(name)) return nullptr;
		return window;
	}

	bool InspectorViewport::Init(const String& name)
	{
		m_name = name;
		m_isOpen = true;
		return true;
	}

	void InspectorViewport::OnDraw()
	{
		if (ImGui::Begin(m_name.c_str(), &m_isOpen))
		{
			// TODO: 나중에 MGF_ENTITY 매니저를 순회하며 실제 엔티티 목록을 출력해야 합니다.
			// 현재는 레이아웃 확인을 위한 더미 노드입니다.
		}
		ImGui::End();
	}
}