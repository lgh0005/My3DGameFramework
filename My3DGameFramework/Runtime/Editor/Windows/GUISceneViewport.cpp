#include "RuntimePch.h"
#include "GUISceneViewport.h"
#include "Managers/RenderManager.h"

namespace MGF3D
{
	SceneViewport::SceneViewport() = default;
	SceneViewport::~SceneViewport() = default;

	SceneViewportUPtr SceneViewport::Create(const String& name)
	{
		auto viewport = SceneViewportUPtr(new SceneViewport());
		if (!viewport->Init(name)) return nullptr;
		return viewport;
	}

	bool SceneViewport::Init(const String& name)
	{
		m_name = name;
		m_isOpen = true;
		return true;
	}

	void SceneViewport::OnDraw()
	{
		// 1. 텍스처가 창의 여백 없이 꽉 차게 렌더링되도록 패딩 제거
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(640.0f, 360.0f), ImGuiCond_FirstUseEver);

		// 2. 창 렌더링 시작 (m_isOpen 포인터를 넘겨 우측 상단 닫기(X) 버튼 활성화)
		if (ImGui::Begin(m_name.c_str(), &m_isOpen))
		{
			// 현재 창 내부의 실제 렌더링 가능한 크기 추출
			ImVec2 viewportSize = ImGui::GetContentRegionAvail();

			// 유효한 텍스처(프레임버퍼 Color Attachment)가 바인딩 되어 있다면 그리기
			uint32 finalSceneTexture = MGF_RENDER.GetFinalSceneTextureID();
			if (finalSceneTexture != 0)
			{
				ImGui::Image
				(
					(ImTextureID)(uintptr_t)finalSceneTexture,
					viewportSize,
					ImVec2(0.0f, 1.0f),
					ImVec2(1.0f, 0.0f)
				);
			}
		}
		ImGui::End();

		// 3. 변경한 스타일 롤백
		ImGui::PopStyleVar();
	}
}