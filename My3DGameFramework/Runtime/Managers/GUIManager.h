#pragma once

struct GLFWwindow;

namespace MGF3D
{
	MGF_CLASS_PTR(GUIWindow)

	class GUIManager
	{
		MGF_DECLARE_SINGLE(GUIManager)

	private:
		GUIManager();
		~GUIManager();

	public:
		void Init(GLFWwindow* window);
		void Render();
		void Shutdown();

	public:
		void AddGUIWindow(GUIWindowUPtr gui);
		void SetGUIDockSpace();

	private:
		Vector<GUIWindowUPtr> m_guis;
	};
}