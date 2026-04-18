#pragma once

struct GLFWwindow;

namespace MGF3D
{
	MGF_CLASS_PTR(MGFWindow)

	class WindowManager
	{
		MGF_DECLARE_SINGLE(WindowManager)

	private:
		WindowManager();
		~WindowManager();

	public:
		bool Init(int32 width, int32 height, const String& title, int32 vsync = 0);
		void Update();
		void Shutdown();

	public:
		bool ShouldClose() const;
		MGFWindow* GetMainWindow() const { return m_mainWindow.get(); }
		GLFWwindow* GetNativeHandle() const;
		MGFWindowUPtr CreateSharedContextWindow() const;

		int32 GetWindowWidth() const;
		int32 GetWindowHeight() const;
		void SwapWindowBuffers();

	private:
		static void ErrorCallback(int32 error, cstr description);
		MGFWindowUPtr m_mainWindow { nullptr };
	};
}