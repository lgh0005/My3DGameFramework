#include "CorePch.h"
#include "MGFWindow.h"
#include "MGFWindowInterface.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace MGF3D
{
	MGFWindow::MGFWindow() : m_window(nullptr) {}
	MGFWindow::~MGFWindow()
	{
		Destroy();
	}

	UniquePtr<MGFWindow> MGFWindow::Create
	(
		int32 width, int32 height, 
		const String& title, int32 vsync,
		bool isVisible, GLFWwindow* share
	)
	{
		auto window = MGFWindowUPtr(new MGFWindow());
		if (!window->Init(width, height, title, vsync, isVisible, share))
		{
			MGF_LOG_ERROR("MGFWindow: Failed to initialize window instance.");
			return nullptr;
		}

		return window;
	}

	MGFWindow* MGFWindow::GetWindowInstance(GLFWwindow* window)
	{
		return static_cast<MGFWindow*>(glfwGetWindowUserPointer(window));
	}

	bool MGFWindow::Init
	(
		int32 width, int32 height, 
		const String& title, int32 vsync,
		bool isVisible, GLFWwindow* share
	)
	{
		// 1. OpenGL 힌트 설정
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_VISIBLE, CommonUtils::Select(isVisible, GLFW_TRUE, GLFW_FALSE));

		// 2. 윈도우 생성
		m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, share);
		glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

		if (!m_window)
		{
			MGF_LOG_FATAL("MGFWindow: Failed to create GLFW window.");
			glfwTerminate();
			return false;
		}
		m_width = width;
		m_height = height;
		glfwSetWindowUserPointer(m_window, this);

		// 3. 컨텍스트 설정 및 인스턴스 연결
		// 메인 윈도우와 공유 컨텍스트를 위한 윈도우 생성 처리에 대한 분기
		if (isVisible)
		{
			glfwMakeContextCurrent(m_window);
			glfwSwapInterval(vsync);
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				MGF_LOG_FATAL("MGFWindow: Failed to initialize GLAD.");
				glfwTerminate();
				return false;
			}

			// 4. 콜백 등록
			MGFWindowInterface::Install(m_window);

			// 5. SPIR-V 지원 체크
			if (glfwExtensionSupported("GL_ARB_gl_spirv")) MGF_LOG_INFO("SPIR-V supported!");
			else MGF_LOG_WARN("SPIR-V not supported on this system!");
		}

		MGF_LOG_INFO("MGFWindow: Window created successfully ({0}x{1})", width, height);
		return true;
	}

	void MGFWindow::Destroy()
	{
		if (m_window)
		{
			glfwDestroyWindow(m_window);
			m_window = nullptr;
			MGF_LOG_INFO("MGFWindow: Window handle destroyed via glfwDestroyWindow.");
		}
	}

	void MGFWindow::SwapBuffers()
	{
		if (m_window) glfwSwapBuffers(m_window);
	}

	/*==================================//
	//   Window status change methods   //
	//==================================*/
	void MGFWindow::UpdateWindowSize(int32 w, int32 h)
	{
		m_width = w; m_height = h; 
	}

	void MGFWindow::UpdateWindowIconified(bool icon)
	{
		m_isIconified = icon;
	}

	void MGFWindow::UpdateWindowFocus(bool focus)
	{
		m_isFocused = focus;
	}

	void MGFWindow::UpdateWindowClose(bool close)
	{
		m_shouldClose = close;
		if (m_window)
		{
			glfwSetWindowShouldClose
			(
				m_window,
				CommonUtils::Select(close, GLFW_TRUE, GLFW_FALSE)
			);
		}
	}

	void MGFWindow::UpdateWindowPos(int32 x, int32 y)
	{
		m_winX = x; m_winY = y;
	}
}