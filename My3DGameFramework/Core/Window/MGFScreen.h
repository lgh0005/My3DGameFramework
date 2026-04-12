#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(MGFScreen)
	class MGFScreen
	{
	public:
		MGFScreen();
		~MGFScreen();

	public:
		bool Init();
		int32 GetRefreshRate() const;
		int32 GetScreenWidth() const;
		int32 GetScreenHeight() const;

	private:
		GLFWmonitor* m_monitor;
		const GLFWvidmode* m_mode;
		
		int32 m_refreshRate{ 60 };
		int32 m_screenWidth{ 0 };
		int32 m_screenHeight{ 0 };
	};
}