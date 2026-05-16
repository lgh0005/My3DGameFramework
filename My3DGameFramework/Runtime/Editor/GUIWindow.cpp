#include "RuntimePch.h"
#include "GUIWindow.h"

namespace MGF3D
{
	GUIWindow::GUIWindow() = default;
	GUIWindow::~GUIWindow() = default;

	void GUIWindow::SetOpen(bool isOpen)
	{
		m_isOpen = isOpen;
	}

	bool GUIWindow::IsOpen() const
	{
		return m_isOpen;
	}
}