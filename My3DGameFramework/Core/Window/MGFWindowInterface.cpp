#include "CorePch.h"
#include "MGFWindowInterface.h"
#include "MGFWindow.h"
#include <GLFW/glfw3.h>

namespace MGF3D
{
	void MGFWindowInterface::Install(GLFWwindow* window)
	{
		if (window == nullptr) return;

		glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
		glfwSetWindowIconifyCallback(window, WindowIconifyCallback);
		glfwSetWindowCloseCallback(window, WindowCloseCallback);
		glfwSetWindowFocusCallback(window, WindowFocusCallback);
		glfwSetWindowPosCallback(window, WindowPosCallback);
	}

	void MGFWindowInterface::FramebufferSizeCallback(GLFWwindow* window, int32 width, int32 height)
	{
		if (width <= 0 || height <= 0) return;
		MGFWindow* instance = MGFWindow::GetWindowInstance(window);
		if (instance != nullptr)
		{
			instance->UpdateWindowSize(width, height);
			MGF_LOG_INFO("MGFWindowInterface: Framebuffer resized to {0}x{1}", width, height);
		}
	}

	void MGFWindowInterface::WindowIconifyCallback(GLFWwindow* window, int32 iconified)
	{
		MGFWindow* instance = MGFWindow::GetWindowInstance(window);
		if (instance != nullptr)
		{
			const bool isIconified = static_cast<bool>(iconified);
			instance->UpdateWindowIconified(isIconified);
			MGF_LOG_INFO("MGFWindowInterface: Window {0}", CommonUtils::Select<String>(isIconified, "Iconified", "Restored"));
		}
	}

	void MGFWindowInterface::WindowCloseCallback(GLFWwindow* window)
	{
		MGFWindow* instance = MGFWindow::GetWindowInstance(window);
		if (instance != nullptr)
		{
			instance->UpdateWindowClose(true);
			MGF_LOG_INFO("MGFWindowInterface: Window close requested.");
		}
	}

	void MGFWindowInterface::WindowFocusCallback(GLFWwindow* window, int32 focused)
	{
		MGFWindow* instance = MGFWindow::GetWindowInstance(window);
		if (instance != nullptr)
		{
			const bool isFocused = static_cast<bool>(focused);
			instance->UpdateWindowFocus(isFocused);
			MGF_LOG_INFO("MGFWindowInterface: Window focus {0}", CommonUtils::Select<String>(isFocused, "Gained", "Lost"));
		}
	}

	void MGFWindowInterface::WindowPosCallback(GLFWwindow* window, int32 xpos, int32 ypos)
	{
		MGFWindow* instance = MGFWindow::GetWindowInstance(window);
		if (instance != nullptr)
		{
			instance->UpdateWindowPos(xpos, ypos);
			MGF_LOG_INFO("MGFWindowInterface: Window moved to ({0}, {1})", xpos, ypos);
		}
	}
}