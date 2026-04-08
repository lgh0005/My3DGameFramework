#include "CorePch.h"
#include "MGFKeyAction.h"

namespace MGF3D
{
	MGFKeyAction::MGFKeyAction(const String& name) : Super{ name } { }
	MGFKeyAction::~MGFKeyAction() = default;

	void MGFKeyAction::AddKeyBinding(int32 keyCode)
	{
		if (!IsMapped(keyCode)) m_mappedKeys.push_back(keyCode);
	}

	bool MGFKeyAction::IsMapped(int32 inputCode) const
	{
		auto it = std::find(m_mappedKeys.begin(), m_mappedKeys.end(), inputCode);
		return it != m_mappedKeys.end();
	}

	bool MGFKeyAction::CheckRawState(GLFWwindow* window) const
	{
		for (int32 key : m_mappedKeys)
		{
			if (glfwGetKey(window, key) == GLFW_PRESS)
				return true;
		}
		return false;
	}
}