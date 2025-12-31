#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(UIRenderer)
#pragma endregion

// TEMP : 임시로 일단 적어본 UI 정렬 레이어
enum class SortLayer
{
	Background, // 배경 이미지
	Default,    // 일반 UI
	HUD,        // 체력바 등
	Popup,      // 팝업 창 (가장 위)
	Cursor,     // 커서
	MAX
};

enum
{

};

struct UIAction
{
	// TODO
};

class UIManager
{
	DECLARE_SINGLE(UIManager)

public:
	using UILayerList = std::vector<UIRenderer*>;
	using UILayerContainer = std::array<UIRenderer*, (usize)SortLayer::MAX>;

public:
	void Update();
	void Clear();

	void RegisterRenderer(UIRenderer* renderer);
	void UnregisterRenderer(UIRenderer* renderer);

	const UILayerContainer& GetScreenSpaceLayers();
	const UILayerList& GetWorldSpaceRenderers();

private:
	void SortRenderers();

	UILayerList m_worldSpaceRenderers;
	UILayerContainer m_screenSpaceRenderers;
	bool m_isUIOrderDirty = true;
};