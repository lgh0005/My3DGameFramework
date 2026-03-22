#pragma once
#include "Platform/Window/MGFWindow.h"

struct GLFWwindow;

namespace MGF3D
{
	class WindowManager
	{
		MGF_DECLARE_SINGLE(WindowManager)

	private:
		WindowManager();
		~WindowManager();

	public:
		bool Init(int32 width, int32 height, const LString& title);
		void Shutdown();
		void Update();

	public:
		bool ShouldClose() const;
		Ptr<MGFWindow> GetMainWindow() const { return m_mainWindow.Get(); }
		Ptr<GLFWwindow> GetNativeWindow() const;
		int32 GetWindowWidth() const;
		int32 GetWindowHeight() const;
		bool IsIconified() const;

	public:
		void SetResizeCallback(const MGFWindow::ResizeCallback& callback);
		void SetIconifyCallback(const MGFWindow::IconifyCallback& callback);

	private:
		UniquePtr<MGFWindow> m_mainWindow{ nullptr };
	};
}