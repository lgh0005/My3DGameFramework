#pragma once
#include "Platform/Input/MGFInputAction.h"
#include "Utils/GLFWInputMapper.h"

namespace MGF3D
{
	class InputManager
	{
		MGF_DECLARE_SINGLE(InputManager)

	private:
		InputManager();
		~InputManager();

	public:
		bool Init(GLFWwindow* window);
		void Update(GLFWwindow* window);
		void Shutdown();

	public:
		void MapAction(strview actionName, KeyCode code);
		void MapMouseAction(strview actionName, MouseCode code);
		void BindAction(strview actionName, InputEvent event, const MGFInputAction::InputCallback& callback);

		bool GetButton(strview actionName);
		bool GetButtonDown(strview actionName);
		bool GetButtonUp(strview actionName);

		const vec2& GetMousePos() const { return m_mousePos; }

	private:
		static void DispatchKey(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods);
		static void DispatchMouse(GLFWwindow* window, int32 button, int32 action, int32 mods);
		static void DispatchCursorPos(GLFWwindow* window, double xpos, double ypos);

		void OnKey(int32 key, int32 scancode, int32 action, int32 mods);
		void OnMouse(int32 button, int32 action, int32 mods);
		void OnCursorPos(double xpos, double ypos);
		
		Ptr<MGFInputAction> FetchAction(strview name);
		Ptr<MGFInputAction> GetAction(strview name);
		bool CheckActionState(const Ptr<MGFInputAction>& action, GLFWwindow* window);

	private:
		SMap<StringHash, int32> m_actionIndexMap;
		SVector<MGFInputActionUPtr> m_actionList;
		SVector<bool> m_currentStates;
		SVector<bool> m_prevStates;

		Array<SVector<Ptr<MGFInputAction>>, GLFW_KEY_LAST + 1> m_keyMap;
		Array<SVector<Ptr<MGFInputAction>>, GLFW_MOUSE_BUTTON_LAST + 1> m_mouseMap;
		vec2 m_mousePos{ 0.0f };
	};
}
