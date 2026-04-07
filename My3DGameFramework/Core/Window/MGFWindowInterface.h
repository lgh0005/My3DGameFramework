#pragma once

struct GLFWwindow;

namespace MGF3D
{
	class MGFWindowInterface
	{
		MGF_DECLARE_UTILITIES(MGFWindowInterface)

	public:
		static void Install(GLFWwindow* window);

	/*============================//
	//      Window Callbacks      //
	//============================*/
	private:
		static void FramebufferSizeCallback(GLFWwindow* window, int32 width, int32 height);
		static void WindowIconifyCallback(GLFWwindow* window, int32 iconified);
		static void WindowCloseCallback(GLFWwindow* window);
		static void WindowFocusCallback(GLFWwindow* window, int32 focused);
		static void WindowPosCallback(GLFWwindow* window, int32 xpos, int32 ypos);
	};
}