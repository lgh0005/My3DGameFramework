#include "EnginePch.h"
#include "UICanvas.h"
#include "Core/GameObject.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Graphics/Program.h"
#include "UIs/UIRenderer.h"

UICanvas::UICanvas() = default;
UICanvas::~UICanvas() = default;

UICanvasUPtr UICanvas::Create(UIRenderMode mode, int32 sortOrder)
{
	auto canvas = UICanvasUPtr(new UICanvas());
	canvas->Init(mode, sortOrder);
	return std::move(canvas);
}

void UICanvas::Init(UIRenderMode mode, int32 sortOrder)
{
	m_renderMode = mode;
	m_canvasSortingOrder = sortOrder;
}

void UICanvas::Render(Program* program)
{
	if (m_isDirtyOrder)
	{
		SortUIElements();
		m_isDirtyOrder = false;
	}

	for (auto ui : m_uiElements)
		ui->Render(program);
}

void UICanvas::AddUIElement(UIRenderer* ui)
{
	m_uiElements.push_back(ui);
}

void UICanvas::RemoveUIElement(UIRenderer* ui)
{
	auto it = std::remove(m_uiElements.begin(), m_uiElements.end(), ui);
	m_uiElements.erase(it, m_uiElements.end());
}

void UICanvas::SortUIElements()
{
	std::sort(m_uiElements.begin(), m_uiElements.end(),
	[](UIRenderer* a, UIRenderer* b) 
	{
		return a->GetSortingOrder() < b->GetSortingOrder();
	});
}