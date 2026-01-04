#include "EnginePch.h"
#include "UICanvas.h"
#include "Scene/GameObject.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Resources/Program.h"
#include "UIs/UIRenderer.h"

DECLARE_DEFAULTS_IMPL(UICanvas)

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
	// 1. 그리기 전에 정렬 상태 확인
	SortUIElements();

	// 2. 순서대로 그리기
	for (auto ui : m_uiElements)
		ui->Render(program);
}

void UICanvas::AddUIElement(UIRenderer* ui)
{
	m_uiElements.push_back(ui);
	m_isUIElementDirtyOrder = true;
}

void UICanvas::RemoveUIElement(UIRenderer* ui)
{
	auto it = std::remove(m_uiElements.begin(), m_uiElements.end(), ui);
	if (it != m_uiElements.end()) m_uiElements.erase(it, m_uiElements.end());
}

void UICanvas::SortUIElements()
{
	// 더티 플래그가 꺼져있으면(false) 아무것도 안 하고 리턴
	if (m_isUIElementDirtyOrder == false) return;

	// 정렬 수행
	std::sort(m_uiElements.begin(), m_uiElements.end(),
	[](UIRenderer* a, UIRenderer* b)
	{
		return a->GetSortingOrder() < b->GetSortingOrder();
	});

	// 정렬 끝났으니 플래그 끄기
	m_isUIElementDirtyOrder = false;
}