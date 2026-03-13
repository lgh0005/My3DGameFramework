#include "MiddlewarePch.h"
#include "MGFWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace MGF3D
{
	MGFWindow::MGFWindow() : m_window(nullptr) { }
	MGFWindow::~MGFWindow()
	{
		Destroy();
	}

	UniquePtr<MGFWindow> MGFWindow::Create(int32 width, int32 height, const LString& title)
	{
		auto window = MGFWindowUPtr(new MGFWindow());
		if (!window->Init(width, height, title))
		{
			MGF_LOG_ERROR("MGFWindow: Failed to initialize window instance.");
			return nullptr;
		}

		return window;
	}

	Ptr<MGFWindow> MGFWindow::GetWindowInstance(Ptr<GLFWwindow> window)
	{
		return static_cast<Ptr<MGFWindow>>(glfwGetWindowUserPointer(window));
	}

	bool MGFWindow::Init(int32 width, int32 height, const LString& title)
	{
		// 1. OpenGL 힌트 설정
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// 2. 윈도우 생성
		m_window = glfwCreateWindow(width, height, title.CStr(), nullptr, nullptr);
		if (!m_window)
		{
			MGF_LOG_FATAL("MGFWindow: Failed to create GLFW window.");
			return false;
		}

		m_width = width;
		m_height = height;

		// 3. 컨텍스트 설정 및 인스턴스 연결
		glfwMakeContextCurrent(m_window);
		glfwSetWindowUserPointer(m_window, this);
		glfwSwapInterval(0); // TEMP : v-sync 끄기
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			MGF_LOG_FATAL("MGFWindow: Failed to initialize GLAD.");
			return false;
		}

		// 4. 콜백 등록
		glfwSetFramebufferSizeCallback(m_window, HandleFramebufferSizeChange);
		glfwSetWindowIconifyCallback(m_window, HandleWindowIconified);

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

	/*=================================//
	//   MGF static window callbacks   //
	//=================================*/
	void MGFWindow::HandleFramebufferSizeChange(Ptr<GLFWwindow> window, int32 width, int32 height)
	{
		if (width <= 0 || height <= 0) return;

		MGFWindow* instance = GetWindowInstance(window);
		if (instance)
		{
			instance->m_width = width;
			instance->m_height = height;

			if (instance->m_resizeCallback)
				instance->m_resizeCallback(width, height);
		}
	}

	void MGFWindow::HandleWindowIconified(Ptr<GLFWwindow> window, int32 iconified)
	{
		MGFWindow* instance = GetWindowInstance(window);
		if (instance)
		{
			instance->m_isIconified = (iconified == GLFW_TRUE);
			if (instance->m_iconifyCallback)
				instance->m_iconifyCallback(instance->m_isIconified);
		}
	}
}