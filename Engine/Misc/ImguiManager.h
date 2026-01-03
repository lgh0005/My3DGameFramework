#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class ImGuiManager
{
	DECLARE_SINGLE(ImGuiManager)
	DECLARE_NONINSTANTIABLE(ImGuiManager)

public:
	void Init(bool enable);
	void Clear();
	void BeginFrame();
	void EndFrame();
	bool Begin(const char* name, 
			   bool* p_open = nullptr, ImGuiWindowFlags flags = 0);
	void End();

private:
	bool m_isEnable	{ true };
};