#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(UICanvas)
#pragma endregion

struct UIAction
{
	// TODO : UI 상호작용 시 수행해야 할 일감
};

class UIManager
{
	DECLARE_SINGLE(UIManager)

public:
	void Init();
	void Update();
	void Clear();

	void RegisterCanvas(UICanvas* canvas);
	void UnregisterCanvas(UICanvas* canvas);

	const std::vector<UICanvas*>& GetScreenSpaceLayers() { return m_screenSpaceCanvases; }
	const std::vector<UICanvas*>& GetWorldSpaceRenderers() { return m_worldSpaceCanvases; }

private:
	void SortCanvases();

	std::vector<UICanvas*> m_worldSpaceCanvases;
	std::vector<UICanvas*> m_screenSpaceCanvases;
	bool m_isUICanvasOrderDirty = true;
};