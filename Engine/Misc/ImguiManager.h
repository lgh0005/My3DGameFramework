#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// TODO : 이후에 ImGuizmo를 이용해서 오브젝트를 
// 유니티처럼 쉽게 조절할 수 있도록 만들 필요가 있다.
// #include "ImGuizmo.h"

class ImGuiManager
{
	DECLARE_SINGLE(ImGuiManager)

public:
	void Init(GLFWwindow* handle, bool enable);
	void ShutDown();
	void BeginFrame();
	void EndFrame();
	bool Begin(const char* name, 
			   bool* p_open = nullptr, ImGuiWindowFlags flags = 0);
	void End();

private:
	bool m_isEnable	{ true };
};