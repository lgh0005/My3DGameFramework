#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class ImGuiManager
{
	DECLARE_SINGLE(ImGuiManager)

public:
	void Init(GLFWwindow* handle, bool enable);
	void ShutDown();
	void BeginFrame();
	void EndFrame();

private:
	bool m_isEnable	{ true };
};