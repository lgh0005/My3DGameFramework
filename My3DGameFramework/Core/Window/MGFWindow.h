#pragma once

struct GLFWwindow;

namespace MGF3D
{
	MGF_CLASS_PTR(MGFScreen)

	MGF_CLASS_PTR(MGFWindow)
	class MGFWindow
	{
	public:
		friend class MGFWindowInterface;
		using ResizeCallback = Action<int32, int32>;
		using IconifyCallback = Action<bool>;
		~MGFWindow();

	public:
		static UniquePtr<MGFWindow> Create
		(
			int32 width, int32 height,
			const String& title, int32 vsync = 0,
			bool isVisible = true, GLFWwindow* share = nullptr
		);
		static MGFWindow* GetWindowInstance(GLFWwindow* window);

	public:
		MGFScreen* GetMonitor() const { return m_screen.get(); }
		GLFWwindow* GetHandle() const { return m_window; }
		int32 GetWidth() const { return m_width; }
		int32 GetHeight() const { return m_height; }
		int32 GetWindowPosX() const { return m_winX; }
		int32 GetWindowPosY() const { return m_winY; }
		bool IsIconified() const { return m_isIconified; }
		bool IsFocused()   const { return m_isFocused; }
		bool ShouldClose() const { return m_shouldClose; }

	public:
		void Destroy();
		void SwapBuffers();

	/*==================================//
	//   Window status change methods   //
	//==================================*/
	private:
		void UpdateWindowSize(int32 w, int32 h);
		void UpdateWindowIconified(bool icon);
		void UpdateWindowFocus(bool focus);
		void UpdateWindowClose(bool close);
		void UpdateWindowPos(int32 x, int32 y);

	private:
		MGFWindow();
		bool Init
		(
			int32 width, int32 height, 
			const String& title, int32 vsync,
			bool isVisible, GLFWwindow* share
		);
		MGFScreenUPtr m_screen	{ nullptr };
		GLFWwindow* m_window	{ nullptr };
		int32 m_width{ 0 },  m_height { 0 };
		int32 m_winX { 0 },  m_winY   { 0 };

		bool m_isIconified  { false };
		bool m_isFocused	{ true };
		bool m_shouldClose  { false };
	};
}