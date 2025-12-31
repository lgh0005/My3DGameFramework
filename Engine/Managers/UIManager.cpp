#include "EnginePch.h"
#include "UIManager.h"
#include "UIs/UICanvas.h"

void UIManager::Init()
{
	// 벡터 메모리 예약
	m_worldSpaceCanvases.reserve(10);
	m_screenSpaceCanvases.reserve(10);
}

void UIManager::Update()
{
	// 1. 캔버스 간의 순서 정렬
	if (m_isUICanvasOrderDirty)
	{
		SortCanvases();
		m_isUICanvasOrderDirty = false;
	}

	// 2. 각 캔버스 내부 요소 정렬 로직 호출
	// Screen Space 캔버스 업데이트
	for (auto canvas : m_screenSpaceCanvases)
		canvas->SortUIElements();

	// World Space 캔버스 업데이트
	for (auto canvas : m_worldSpaceCanvases)
		canvas->SortUIElements();
}

void UIManager::RegisterCanvas(UICanvas* canvas)
{
	if (canvas == nullptr) return;

	// 렌더 모드에 따라 적절한 리스트에 등록
	switch (canvas->GetUIRenderMode())
	{
		case UIRenderMode::ScreenSpace:
		{
			m_screenSpaceCanvases.push_back(canvas);
			break;
		}

		case UIRenderMode::WorldSpace:
		{
			m_worldSpaceCanvases.push_back(canvas);
			break;
		}
	}

	m_isUICanvasOrderDirty = true;
}

void UIManager::UnregisterCanvas(UICanvas* canvas)
{
	if (canvas == nullptr) return;

	// 렌더 모드에 따라 적절한 리스트에 등록
	switch (canvas->GetUIRenderMode())
	{
		case UIRenderMode::ScreenSpace:
		{
			auto it = std::remove
			(
				m_screenSpaceCanvases.begin(), 
				m_screenSpaceCanvases.end(), 
				canvas
			);
			if (it != m_screenSpaceCanvases.end())
				m_screenSpaceCanvases.erase(it, m_screenSpaceCanvases.end());
			break;
		}

		case UIRenderMode::WorldSpace:
		{
			auto it = std::remove
			(
				m_worldSpaceCanvases.begin(), 
				m_worldSpaceCanvases.end(), 
				canvas
			);
			if (it !=m_worldSpaceCanvases.end())
				m_worldSpaceCanvases.erase(it, m_worldSpaceCanvases.end());
			break;
		}
	}
}

void UIManager::SortCanvases()
{
	// Screen Space 정렬
	if (m_screenSpaceCanvases.size() > 1)
	{
		std::sort(m_screenSpaceCanvases.begin(), m_screenSpaceCanvases.end(),
		[](UICanvas* a, UICanvas* b)
		{
			return a->GetUICanvasSortingOrder() < b->GetUICanvasSortingOrder();
		});
	}

	// World Space 정렬
	if (m_worldSpaceCanvases.size() > 1)
	{
		std::sort(m_worldSpaceCanvases.begin(), m_worldSpaceCanvases.end(),
		[](UICanvas* a, UICanvas* b)
		{
			return a->GetUICanvasSortingOrder() < b->GetUICanvasSortingOrder();
		});
	}
}

void UIManager::Clear()
{
	m_screenSpaceCanvases.clear();
	m_worldSpaceCanvases.clear();
	m_isUICanvasOrderDirty = false;
}
