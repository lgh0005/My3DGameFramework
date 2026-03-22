#pragma once
#include "Pointer/PoolAlloc.h"

struct GLFWwindow;

namespace MGF3D
{
	MGF_CLASS_PTR(MGFWindow)
	class MGFWindow : public PoolAlloc
	{
	public:
		using ResizeCallback = Action<int32, int32>;
		using IconifyCallback = Action<bool>;
		~MGFWindow();

	public:
		static UniquePtr<MGFWindow> Create(int32 width, int32 height, const LString& title);
		static Ptr<MGFWindow> GetWindowInstance(Ptr<GLFWwindow> window);
		void Destroy();
		void SwapBuffers();

		Ptr<GLFWwindow> GetHandle() const { return m_window; }
		int32 GetWidth() const { return m_width; }
		int32 GetHeight() const { return m_height; }
		bool IsIconified() const { return m_isIconified; }

		void SetResizeCallback(const ResizeCallback& callback) { m_resizeCallback = callback; }
		void SetIconifyCallback(const IconifyCallback& callback) { m_iconifyCallback = callback; }

	private:
		static void HandleFramebufferSizeChange(Ptr<GLFWwindow> window, int32 width, int32 height);
		static void HandleWindowIconified(Ptr<GLFWwindow> window, int32 iconified);

	private:
		MGFWindow();
		bool Init(int32 width, int32 height, const LString& title);
		Ptr<GLFWwindow> m_window{ nullptr };
		int32 m_width{ 0 };
		int32 m_height{ 0 };
		bool m_isIconified{ false };

		ResizeCallback  m_resizeCallback;
		IconifyCallback m_iconifyCallback;
	};
}