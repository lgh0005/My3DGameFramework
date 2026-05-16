#include "RuntimePch.h"
#include "GUIHierarchyViewport.h"

namespace MGF3D
{
	HierarchyViewport::HierarchyViewport() = default;
	HierarchyViewport::~HierarchyViewport() = default;

	HierarchyViewportUPtr HierarchyViewport::Create(const String& name)
	{
		auto window = HierarchyViewportUPtr(new HierarchyViewport());
		if (!window->Init(name))
		{
			return nullptr;
		}
		return window;
	}

	bool HierarchyViewport::Init(const String& name)
	{
		m_name = name;
		m_isOpen = true;
		return true;
	}

	void HierarchyViewport::OnDraw()
	{
		if (ImGui::Begin(m_name.c_str(), &m_isOpen))
		{
			// 화살표 영역을 클릭했을 때만 트리가 열리도록 플래그를 설정합니다.
			ImGuiTreeNodeFlags rootFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

			if (ImGui::TreeNodeEx("World Scene", rootFlags))
			{
				// 레이아웃 검증을 위한 정적 변수입니다. 추후 전역 선택 매니저 데이터로 대체됩니다.
				static int selectedIndex = -1;

				// 각 항목 배치 및 선택 상태 시각화
				if (ImGui::Selectable("Alien Soldier", selectedIndex == 0, ImGuiSelectableFlags_SpanAllColumns))
				{
					selectedIndex = 0;
				}
				if (ImGui::Selectable("Main Camera", selectedIndex == 1, ImGuiSelectableFlags_SpanAllColumns))
				{
					selectedIndex = 1;
				}
				if (ImGui::Selectable("Directional Light", selectedIndex == 2, ImGuiSelectableFlags_SpanAllColumns))
				{
					selectedIndex = 2;
				}
				if (ImGui::Selectable("SkyLight", selectedIndex == 3, ImGuiSelectableFlags_SpanAllColumns))
				{
					selectedIndex = 3;
				}

				ImGui::TreePop();
			}
		}
		ImGui::End();
	}
}