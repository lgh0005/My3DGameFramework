#include "CorePch.h"
#include "MGFScreen.h"

namespace MGF3D
{
	MGFScreen::MGFScreen() = default;
	MGFScreen::~MGFScreen() = default;

	bool MGFScreen::Init()
	{
		// 1. 현재 연결된 모니터 정보를 가져온다.
		m_monitor = glfwGetPrimaryMonitor();
		if (!m_monitor)
		{
			MGF_LOG_ERROR("MGFScreen : Failed to create GLFWmonitor instance.");
			return false;
		}

		// 2. 비디오 모드 설정
		m_mode = glfwGetVideoMode(m_monitor);
		if (!m_mode)
		{
			MGF_LOG_ERROR("MGFScreen : Failed to get video mode of GLFWmonitor.");
			return false;
		}
		
		m_refreshRate = m_mode->refreshRate;
		m_screenWidth = m_mode->width;
		m_screenHeight = m_mode->height;

		return true;
	}

	int32 MGFScreen::GetRefreshRate() const { return m_refreshRate; }
	int32 MGFScreen::GetScreenWidth() const { return m_screenWidth; }
	int32 MGFScreen::GetScreenHeight() const { return m_screenHeight; }
}