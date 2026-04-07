#pragma once

struct GLFWwindow;

namespace MGF3D
{
	class MGFInputInterface
	{
		MGF_DECLARE_UTILITIES(MGFInputInterface)

	public:
		static void Install(GLFWwindow* window);

	public:
		/*==============================//
		//      Keyboard Callbacks      //
		//==============================*/
		static void KeyCallback(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods);
		static void CharCallback(GLFWwindow* window, uint32 codepoint);

		/*==============================//
		//       Mouse Callbacks        //
		//==============================*/
		static void MouseButtonCallback(GLFWwindow* window, int32 button, int32 action, int32 mods);
		static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
		static void CursorEnterCallback(GLFWwindow* window, int32 entered);
		static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

		/*==============================//
		//      Joystick Callbacks      //
		//==============================*/
		static void JoystickCallback(int32 jid, int32 event);

		/*==============================//
		//       System Callbacks       //
		//==============================*/
		static void DropCallback(GLFWwindow* window, int32 count, const char** paths);
		static void WindowFocusCallback(GLFWwindow* window, int32 focused);

	};
}