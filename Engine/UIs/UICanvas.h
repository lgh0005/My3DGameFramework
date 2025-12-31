#pragma once
#include "Components/Component.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(UIRenderer)
#pragma endregion

enum class UIRenderMode
{
    ScreenSpace, // 2D 화면 (Overlay)
    WorldSpace   // 3D 공간 (Billboarding etc)
};

CLASS_PTR(UICanvas)
class UICanvas : public Component
{
public:
    virtual ~UICanvas();
    static UICanvasUPtr Create(UIRenderMode mode = UIRenderMode::ScreenSpace, int32 sortOrder = 0);
    static const ComponentType s_ComponentType = ComponentType::UICanvas;
    virtual ComponentType GetComponentType() const override { return ComponentType::UICanvas; }
    UIRenderMode GetUIRenderMode() const { return m_renderMode; }
    void Render(Program* program);

    void AddUIElement(UIRenderer* ui);
    void RemoveUIElement(UIRenderer* ui);
    void SortUIElements();

    void SetUICanvasSortingOrder(int32 order) { m_canvasSortingOrder = order; }
    int32 GetUICanvasSortingOrder() const { return m_canvasSortingOrder; }

private:
    UICanvas();
    void Init(UIRenderMode mode, int32 sortOrder);

    UIRenderMode m_renderMode { UIRenderMode::ScreenSpace };
    std::vector<UIRenderer*> m_uiElements;
    int32 m_canvasSortingOrder    { 0 };
    bool m_isUIElementDirtyOrder  { true };
};