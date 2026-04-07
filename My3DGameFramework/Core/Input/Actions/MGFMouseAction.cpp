#include "CorePch.h"
#include "MGFMouseAction.h"
#include <GLFW/glfw3.h>

namespace MGF3D
{
	MGFMouseAction::MGFMouseAction(const String& name) : Super{ name } { }
	MGFMouseAction::~MGFMouseAction() = default;

	void MGFMouseAction::AddMouseButtonBinding(int32 button)
	{
		if (!IsMapped(button)) m_mappedMouseButtons.push_back(button);
	}

	bool MGFMouseAction::IsMapped(int32 inputCode) const
	{
		auto it = std::find(m_mappedMouseButtons.begin(), m_mappedMouseButtons.end(), inputCode);
		return it != m_mappedMouseButtons.end();
	}

	bool MGFMouseAction::CheckRawState(GLFWwindow* window) const
	{
		for (int32 btn : m_mappedMouseButtons)
		{
			if (glfwGetMouseButton(window, btn) == GLFW_PRESS)
				return true;
		}
		return false;
	}
}